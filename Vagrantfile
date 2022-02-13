# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "generic/ubuntu2004"
  config.vm.synced_folder ".", "/vagrant"

  # VM1 configuration (webserver)
  config.vm.define "web" do |web|
    web.vm.hostname = "web"
    web.vm.provision "shell", path: "scripts/web.sh"
    web.vm.provider :virtualbox do |vb| vb.name = "web" end
    web.vm.network "private_network", ip: "192.168.60.100"
  end
  # VM2 configuration (normal machine)
  config.vm.define "normal" do |normal|
    normal.vm.hostname = "normal"
    normal.vm.provision "shell", path: "scripts/normal.sh"
    normal.vm.provider :virtualbox do |vb| vb.name = "normal" end
    normal.vm.network "private_network", ip: "192.168.60.101"
  end
  # VM3 configuration (hacker machine)
  config.vm.define "hacker" do |hacker|
    hacker.vm.hostname = "hacker"
    hacker.vm.provision "shell", path: "scripts/hacker.sh"
    hacker.vm.provider :virtualbox do |vb| vb.name = "hacker" end
    hacker.vm.network "private_network", ip: "192.168.60.102"
  end
end
