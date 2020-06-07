Hermes
=======
Setup
-------

In order to use Hermes, you need to have OVS installed already, with a bridge called br0. To do that, you can use the following commands (make sure to replace $INTERFACE_NAME and $IP_ADDRESS with their actual vales):

```bash
	sudo apt-get update  
	sudo apt-get install openvswitch-switch -y  
	sudo ovs-vsctl init  
	sudo ovs-vsctl add-br br0  
	sudo ovs-vsctl add-port br0 $INTERFACE_NAME  
	sudo ifconfig br0 $IP_ADDRESS netmask 255.255.255.0 up  
	sudo ifconfig $INTERFACE_NAME 0  
```

After that, you can just use 
```bash
make
```
to compile the code and generate the excutables.

