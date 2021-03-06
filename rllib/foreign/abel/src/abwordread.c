
// This is an example program for communicating with an Allen Bradley PLC-5 over
// Ethernet.  This is only to demonstrate the methodology needed.  This program is not
// necessarily useful for any other purpose.

//  Copyright (1999) by Ron Gage 

#include <libabplc5.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <math.h>


int main (int argc, char *argv[])
{
struct _comm comm;
struct _data data;
struct namedata name;
struct plc5stat status;
	int count,x,sts,extsts,type,bit;
	unsigned int temp1, temp2;
	count=0;
	if (argc == 3)
		count = 1;
	if (argc == 4)
		count = atoi(argv[3]);
	if ((argc < 3) || (argc > 4))
		{
		printf ("\nThis program will read a register from an Ethernet connected Allen Bradley\n");
		printf ("PLC-5.  It should also work with a Pyramid Integrator.\n\n");
		printf ("This program will not work on a SLC-500.\n\n");
		printf ("Correct Usage:\nabwordread <plc ip addr> <plc register> {<quantity>}\n");
		printf ("abwordread 192.168.10.5 n7:0   - will read one integer from N7:0 on 192.168.10.5\n");
		printf ("abwordread plc n7:30 4 - will read four integers from N7:30 on plc\n"); 
		printf ("\n\n");
		exit (-1);
		}

	comm=abel_attach(argv[1],FALSE);
	if (comm.error != 0)
		{
		printf ("Could not connect.  Check your address and try again.\n");
		exit (-1);
		}
	status = getstatus (comm,FALSE);
	type = PLC5;
	if (status.type == 0xde)
		type = PLC5250;
	comm.tns = comm.tns + 4;
	name = nameconv5 (argv[2],type,FALSE);
	
	data=word_read(comm,argv[2],count,type,FALSE);
	if (data.len == -1)
		{
		sts = data.data[0];
		extsts = data.data[1];
		printf ("An error occured.  The PLC STS byte is %02X, the EXT STS byte is %02X\n",sts,extsts);
		if (sts != 0xf0)
			printf ("Primary Error code is %s\n",errors[(sts/16)]);
		if (extsts != 0)
			printf ("Extended error code is %s\n",ext_errors[extsts]);
		close (comm.handle);
		exit (-1);
		}
	if (type !=PLC5250)
		switch (data.name.section)
			{
			case 0:
				if (data.name.floatdata == TRUE)
					{
					for (x=0;x<data.len;x=x+2)
						{
						temp1 = (data.data[x]);
						temp2 = (data.data[x+1]);
						printf ("%f\n",itof(temp1,temp2));
						}
					}
				if (data.name.floatdata == FALSE)
					{
					if (name.bit == -1)
					for (x=0;x<(data.len);x++)
						printf ("%d\n",(short)data.data[x]);
					if (name.bit != -1)
						{
						bit = data.data[0] && (pow(2,name.bit));
						if (bit == 0)
							printf ("0");
						else
							printf ("1");
						}
					
					}
				break;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				printf ("section = %d\n",data.name.section);
				for (x=0;x<data.len;x++)
					printf ("%02X  ",(byte)data.data[x]);
				printf ("\n");
			}
	if (type ==PLC5250)
		{
		if (data.name.floatdata == TRUE)
			{
			for (x=0;x<data.len;x=x+2)
				{
				temp1 = (data.data[x]);
				temp2 = (data.data[x+1]);
				printf ("%f\n",itof(temp1,temp2));
				}
			}
		if (data.name.floatdata == FALSE)
			{
			for (x=0;x<(data.len);x++)
				printf ("%d\n",(short)data.data[x]);
			}
		}
				 
	close (comm.handle);
	exit (0);
}




