#include<sstream>
#include"quaverstream.h"
using namespace std;
static const short drawmap[128]=
{
	0,2,4,5,7,9,11,12,14,16,17,19,21,23,24,26,28,29,
	31,33,35,36,38,40,41,43,45,47,48,50,52,53,55,57,
	59,60,62,64,65,67,69,71,72,74,76,77,79,81,83,84,
	86,88,89,91,93,95,96,98,100,101,103,105,107,108,
	110,112,113,115,117,119,120,122,124,125,127,1,3,
	6,8,10,13,15,18,20,22,25,27,30,32,34,37,39,42,44,
	46,49,51,54,56,58,61,63,66,68,70,73,75,78,80,82,
	85,87,90,92,94,97,99,102,104,106,109,111,114,116,
	118,121,123,126,
},genkeyx[]={0,12,18,33,36,54,66,72,85,90,105,108};
void CANVAS::initcanvas(string arg)
{
	int i;
	stringstream conv;
	conv<<"ffmpeg -y -hide_banner -f rawvideo -pix_fmt rgba";
	conv<<" -s "<<canvasw<<"x"<<canvash<<" -r "<<fps;
	conv<<" -i - -vf vflip -pix_fmt yuv420p -crf 12 "<<arg;
	ffpipe=popen(conv.str().c_str(),"wb");
	candat=new unsigned int[canvash*canvasw];
	for(i=0;i<128;++i)
		keyx[i]=(i/12*126+genkeyx[i%12])*canvasw/1350;
	for(i=0;i<127;++i)
		switch(i%12)
		{
		case(1):case(3):case(6):
		case(8):case(10):
			keyw[i]=canvasw*9/1350;
			break;
		case(4):case(11):
			keyw[i]=keyx[i+1]-keyx[i];
			break;
		default:
			keyw[i]=keyx[i+2]-keyx[i];
		}
	keyw[127]=canvasw-keyx[127];
}
void CANVAS::canvasdestroy()
{
	delete[]candat;
	pclose(ffpipe);
}
void CANVAS::canvasclear()
{
	int i,j;
	for(i=0;i<128;++i)
		switch(i%12)
		{
		case(1):case(3):case(6):
		case(8):case(10):
			keycolor[i]=0xFF000000;
			break;
		default:
			keycolor[i]=0xFFFFFFFF;
		}
	for(i=0;i<canvash;++i)
		for(j=0;j<canvasw;++j)
			candat[i*canvasw+j]=0xFF000000;
}
void CANVAS::writeframe()const
{
	fwrite(candat,canvasw*canvash,4,ffpipe);
}
void CANVAS::drawkeys()
{
	int i,j,bh=keyh*64/100;
	for(i=0;i<75;++i)
	{
		j=drawmap[i];
		fillrect(keyx[j],0,keyw[j],keyh,keycolor[j]);
		rect(keyx[j],0,keyw[j]+1,keyh,0xFF000000);
	}
	while(i<128)
	{
		j=drawmap[i++];
		fillrect(keyx[j],keyh-bh,keyw[j],bh,keycolor[j]);
		rect(keyx[j],keyh-bh,keyw[j]+1,bh,0xFF000000);
	}
	fillrect(0,keyh-2,canvasw,5,0xFF000080);
}
void CANVAS::drawnote(short k,int y,int h,unsigned int c)
{
	if(h>5)--h;
	if(h<1)h=1;
	fillrect(keyx[k]+1,y,keyw[k]-1,h,c);
}
void CANVAS::rect(int x,int y,int w,int h,unsigned int c)
{
	int i;
	if(x<canvasw)
		for(i=y;i<y+h;++i)
			candat[i*canvasw+x]=c;
	if(y<canvash)
		for(i=x;i<x+w;++i)
			candat[y*canvasw+i]=c;
	if(w>1)
		for(i=y;i<y+h;++i)
			candat[i*canvasw+x+w-1]=c;
	if(h>1)
		for(i=x;i<x+w;++i)
			candat[(y+h-1)*canvasw+i]=c;
}
void CANVAS::fillrect(int x,int y,int w,int h,unsigned int c)
{
	int i,j;
	for(i=x;i<x+w;++i)
		for(j=y;j<y+h;++j)
			candat[j*canvasw+i]=c;
}
