#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#include <linux/fs.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/kobject.h>

#define MODULE_NAME "netfilter"

static struct kobject *mymodule;
static unsigned int mystate = 1;

static ssize_t mystate_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", mystate);
}

static ssize_t mystate_store(struct kobject *kobj, struct kobj_attribute *attr, char *buf, size_t count)
{
	sscanf(buf, "%u", &mystate);
	return count;
}

static struct kobj_attribute mystate_attr = __ATTR(mystate, 0660, mystate_show, (void*)mystate_store);

/*
 * Reference:
 *  - https://elixir.bootlin.com/linux/v5.4/source/include/linux/netfilter.h#L79
 */
static struct nf_hook_ops *nfhoIN = NULL;

/*
 * [> Responses from hook functions. <]
 * #define NF_DROP 0
 * #define NF_ACCEPT 1
 * #define NF_STOLEN 2
 * #define NF_QUEUE 3
 * #define NF_REPEAT 4
 * #define NF_STOP 5
 * #define NF_MAX_VERDICT NF_STOP
 */
static unsigned int
hfuncIN(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct net_device *ndIn = state->in;
    struct iphdr *iphdr = NULL;
    struct tcphdr *tcphdr = NULL;
    unsigned int action = NF_ACCEPT;
    unsigned int port = 0;

    /* Parse the packet & drop packets go to 80 port */
    iphdr = ip_hdr(skb);
    if (iphdr->protocol == IPPROTO_TCP) {
        tcphdr = tcp_hdr(skb);
        port = ntohs(tcphdr->dest);
        if (port == 80 && mystate) {
            action = NF_DROP;
            pr_info("%s: Drop packet from %s going to port %u\n",
					MODULE_NAME, ndIn->name, port);
        }
    }

    return action;
}

static int __init mymodule_init(void)
{
	int error = 0;

    /* Allocate space for netfilter hook initialization */
    nfhoIN = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);
    if (nfhoIN == NULL) {
        goto nf_hook_fail;
    }

    /*
     * Initialize netfilter hook
     *  - pf => protocol family
     *  - hook => hook function
     *  - hooknum => which hookpoint should the hook function should attach to
     *  - priority => priority of the hook function
     */
    nfhoIN->pf = PF_INET;                   // IPv4
    nfhoIN->hook = (nf_hookfn*)hfuncIN;     // hook function
    nfhoIN->hooknum = NF_INET_LOCAL_IN;     // received packets
    nfhoIN->priority = NF_IP_PRI_FIRST;     // max hook priority

    /* Register the netfilter hook on specific network namespace */
    nf_register_net_hook(&init_net, nfhoIN);

	/* Create kobject representing the state of the module */
	mymodule = kobject_create_and_add(MODULE_NAME, kernel_kobj);
	if (!mymodule) {
		goto sys_add_fail;
	}

	/* Export module state under sysfs */
	error = sysfs_create_file(mymodule, &mystate_attr.attr);
	if (error) {
		goto sys_add_fail;
	}

	pr_info("%s: sysfs node create under /sys/kernel/%s\n",
			MODULE_NAME, MODULE_NAME);
    return 0;

nf_hook_fail:
    return -1;

sys_add_fail:
    nf_unregister_net_hook(&init_net, nfhoIN);
    kfree(nfhoIN);
	return -ENOMEM;
}

static void __exit mymodule_exit(void)
{
	kobject_put(mymodule);
    nf_unregister_net_hook(&init_net, nfhoIN);
    kfree(nfhoIN);
}

module_init(mymodule_init);
module_exit(mymodule_exit);
MODULE_LICENSE("GPL");
