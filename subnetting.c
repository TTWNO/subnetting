#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

enum NetworkClass {CLASS_A, CLASS_B, CLASS_C};

// This section of code consitutes a more Java-like version of finding a preix
// A C programmer is more likely to simply do the binary math.
const unsigned int prefix_map[9] = {0, 128, 192, 224, 240, 248, 252, 254, 255};
const int usable_hosts_map[9] = {254, 126, 62, 30, 14, 6, 2, 0, 0};
const int subnet_multiplyer[4] = {0, 8, 16, 24};

unsigned int* subnet_to_octet_array(unsigned int octets[4], char subnet_mask[16]){
	sscanf(subnet_mask, "%u.%u.%u.%u", &octets[0], &octets[1], &octets[2], &octets[3]);
	return octets;
}

int mask_to_prefix(char mask[16]){
	unsigned int octet[4];
	int pref = 0;
	subnet_to_octet_array(octet, mask);
	for (int i = 0; i < 4; ++i){
		for (int j = 0; j < 9; ++j){
			if (octet[i] == prefix_map[j]){
				pref += j;
				break;
			}
		}
	}
	return pref;
}

char* prefix_to_mask(char subnet_mask[16], unsigned int prefix){
	unsigned int octets[5];
	for (int i = 0; i < 5; ++i){
		for (int j = 8; j >= 0; --j){
			if (prefix >= j){
				prefix -= j;
				octets[i] = prefix_map[j];
				break;
			}
		}
	}
	sprintf(subnet_mask, "%u.%u.%u.%u", octets[0], octets[1], octets[2], octets[3]);
	return subnet_mask;
}

int hosts_for_prefix(int prefix){
	return (int)pow(2.0, (double)(32-prefix));
}

int subnets_for_prefix(int prefix){
	return (int)pow(2.0, (double)(prefix%8));
}

void ui_to_octets(char addr[16], unsigned int subnet_mask){
	int octets[4];
	octets[0] = (subnet_mask >> 24) & 0xFF;
	octets[1] = (subnet_mask >> 16) & 0xFF;
	octets[2] = (subnet_mask >> 8) & 0xFF;
	octets[3] = subnet_mask & 0xFF;
	sprintf(addr, "%d.%d.%d.%d", octets[0], octets[1], octets[2], octets[3]);
}	

int prefix_for_hosts(int hosts_required){
	for (int i = 32; i >= 0; --i){
		if (hosts_for_prefix(i)-2 >= hosts_required){
			return i;
		}
	}
	return -1;
}

char* octet_array_to_string(char subnet_mask[16], char octets[4]){
	sprintf(subnet_mask, "%d.%d.%d.%d", octets[0], octets[1], octets[2], octets[3]);
	return subnet_mask;
}

int network_type_based_on_subnet(int subnet_arr[4]){
	if (subnet_arr[0] != 255){
		return -1;
	} else if (subnet_arr[1] != 255){
		return CLASS_A;
	} else if (subnet_arr[2] != 255){
		return CLASS_B;
	} else if (subnet_arr[3] != 255){
		return CLASS_C;
	} else {
		return -1;
	}
}

char network_class(int subnet_arr[4]){
	if (subnet_arr[0] != 255){
		return '?';
	} else if (subnet_arr[1] != 255){
		return 'A';
	} else if (subnet_arr[2] != 255){
		return 'B';
	} else if (subnet_arr[3] != 255){
		return 'C';
	} else {
		return '?';
	}
}

unsigned int octets_to_ui(char octets[16]){
	int so_s[4];
	unsigned int rv;
	sscanf(octets, "%d.%d.%d.%d", &so_s[0], &so_s[1], &so_s[2], &so_s[3]);
	rv = (so_s[0] << 24) | (so_s[1] << 16) | (so_s[2] << 8) | (so_s[3]);
	return rv;
}

unsigned int bdr_addr(unsigned int ip, unsigned int subnet){
	return ip | ~subnet;
}
unsigned int net_addr(unsigned int ip, unsigned int subnet){
	return ip & subnet;
}

char* network_address(char result[16], char ip[16], char subnet[16]){
	unsigned int num = octets_to_ui(ip) & octets_to_ui(subnet);
	ui_to_octets(result, num);
	return result;
}
char* broadcast_address(char result[16], char ip[16], char subnet[16]){
	unsigned int bdr_add = octets_to_ui(ip) | ~octets_to_ui(subnet);
	ui_to_octets(result, bdr_add);
	return result;
}


char* usable_host_addresses(char result[33], char ip[16], char subnet[16]){
	char slow_addrs[16];
	char shigh_addrs[16];
	unsigned int low_addr = net_addr(octets_to_ui(ip), octets_to_ui(subnet));
	unsigned int high_addr = bdr_addr(octets_to_ui(ip), octets_to_ui(subnet));

	++low_addr;
	--high_addr;

	ui_to_octets(slow_addrs, low_addr);
	ui_to_octets(shigh_addrs, high_addr);
	sprintf(result, "%s-%s", slow_addrs, shigh_addrs);
	return result;
}

int get_ui_class(unsigned int ip){
	unsigned int fo = ip >> 24;
	if (fo >= 1 && fo <= 126){
		return CLASS_A;
	} else if (fo >= 128 && fo <= 191){
		return CLASS_B;
	} else if (fo >= 192 && fo <= 223){
		return CLASS_C;
	}
	return -1;
}

int main(){
	char* mask1 = {"255.255.255.192"};
	char mask2[16];
	int prefix, hosts, networks;
	unsigned int octets[4];

	prefix = mask_to_prefix(mask1);
	fprintf(stdout, "Prefix of %s is %d\n", mask1, prefix);
	prefix_to_mask(mask2, prefix);
	fprintf(stdout, "Subnet of %d is %s\n", prefix, mask2);
	hosts = hosts_for_prefix(prefix);
	printf("It can support up to %d hosts.\n", hosts);
	networks = subnets_for_prefix(prefix);
	printf("It can support ip to %d subnets.\n", networks);
	subnet_to_octet_array(octets, mask1);
	for (int i = 0; i < 4; ++i){
		printf("\tPt %d: %d\n", i+1, octets[i]);
	}

	int prefix64,prefix2000;
	char subnet64[16];
	char subnet2000[16];

	int subarr64[4];
	int subarr2000[4];

	prefix64 = prefix_for_hosts(64);
	prefix2000 = prefix_for_hosts(2000);
	prefix_to_mask(subnet64, prefix64);
	prefix_to_mask(subnet2000, prefix2000);
	subnet_to_octet_array(subarr64, subnet64);
	subnet_to_octet_array(subarr2000, subnet2000);
	printf("64 hosts requires a prefix of %d (%s).\n", prefix64, subnet64);
	printf("2000 hosts requires a prefix of %d (%s).\n", prefix2000, subnet2000);
	printf("A prefix of %d can support %d hosts.\n", prefix2000, hosts_for_prefix(prefix2000));
	printf("%s indicates a class %c network.\n", subnet64, network_class(subarr64));
	printf("%s indicates a class %c network.\n", subnet2000, network_class(subarr2000));
	

	char* ip1 = {"192.168.16.122"};
	char* sub1_ip1 = { "255.255.255.240" };
	char na_ip1[16];
	char ba_ip1[16];
	char ra_ip1[33];
	network_address(na_ip1, ip1, sub1_ip1);
	broadcast_address(ba_ip1, ip1, sub1_ip1);
	usable_host_addresses(ra_ip1, ip1, sub1_ip1);
	printf("%s (%s)'s network address is %s.\n", ip1, sub1_ip1, na_ip1);
	printf("%s (%s)'s broadcast address is %s.\n", ip1, sub1_ip1, ba_ip1);
	printf("The host range is %s\n", ra_ip1);
	char* ip2 = {"172.17.130.222"};
	char* sub2_ip2 = { "255.255.128.0" };
	char na_ip2[16];
	network_address(na_ip2, ip2, sub2_ip2);
	printf("%s (%s)'s network address is %s.\n", ip2, sub2_ip2, na_ip2);

	// actual program
	char* print_format = {"%18s | %33s | %18s | %16s | /%2d\n"};
	int num_of_networks;
	int hosts_per_network;
	char file_location[255];
	FILE* print_file;
	printf("Where would you like to save these tables? ");
	scanf("%s", file_location);
	print_file = fopen(file_location, "w");
	fprintf(print_file, "---------------------------------------------------------------------------------------------------------------------\n");
	printf("How many networks do you need? ");
	scanf("%d", &num_of_networks);
	char ip[16];
	unsigned int ui_ip;

	printf("Enter a base IP address to start configuring. (Usually 192.168.x.0, 172.17.x.0, etc...)\nBase IP: ");
	scanf("%s", ip);
	ui_ip = octets_to_ui(ip);
	fprintf(print_file, print_format, "Network Address", "Host Range", "Broadcast Address", "Subnet Mask", 0);
	for (int i = 0; i < num_of_networks; ++i){
		int hosts;
		char subnt_s[16];
		char broadcast_addr[16];
		char network_addr[16];
		char usable_range[33];
		unsigned int subnt;
		int this_prefix;
		printf("How many hosts do you need on this network [%d]? ", i);
		scanf("%d", &hosts);
		this_prefix = prefix_for_hosts(hosts);
		prefix_to_mask(subnt_s, this_prefix);
		subnt = octets_to_ui(subnt_s);
		broadcast_address(broadcast_addr, ip, subnt_s);
		network_address(network_addr, ip, subnt_s);
		usable_host_addresses(usable_range, ip, subnt_s);
		fprintf(print_file, print_format, network_addr, usable_range, broadcast_addr, subnt_s, this_prefix);
		ui_ip = octets_to_ui(broadcast_addr) + 1;	
		ui_to_octets(ip, ui_ip);
	}
	return 0;
}
