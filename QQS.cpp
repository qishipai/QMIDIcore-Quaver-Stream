#include<iostream>
#include<sstream>
#include<cstdlib>
#include<conio.h>
#include"quaverstream.h"
#define ARGTC 9
#define COLORC 96
using namespace std;
using namespace QQS;
const char argts[ARGTC][6]=
{
 "-mid","-vid","-wei",
 "-hei","-keyh","-fps",
 "-ppb","-fthr","-codr",
};
static const unsigned int color[COLORC]=
{
	0xFF0000FF,0xFF00FFFF,0xFF00FF00,0xFFFFFF00,0xFFFF0000,0xFFFF00FF,
	0xFF0040FF,0xFF00FFC0,0xFF40FF00,0xFFFFC000,0xFFFF0040,0xFFC000FF,
	0xFF0080FF,0xFF00FF80,0xFF80FF00,0xFFFF8000,0xFFFF0080,0xFF8000FF,
	0xFF00C0FF,0xFF00FF40,0xFFC0FF00,0xFFFF4000,0xFFFF00C0,0xFF4000FF,
	0xFF0000CF,0xFF00CFFF,0xFF00CF00,0xFFCFFF00,0xFFCF0000,0xFFFF00CF,
	0xFF0040CF,0xFF00CFC0,0xFF40CF00,0xFFCFC000,0xFFCF0040,0xFFC000CF,
	0xFF0080CF,0xFF00CF80,0xFF80CF00,0xFFCF8000,0xFFCF0080,0xFF8000CF,
	0xFF00C0CF,0xFF00CF40,0xFFC0CF00,0xFFCF4000,0xFFCF00C0,0xFF4000CF,
	0xFF00008F,0xFF008FFF,0xFF008F00,0xFF8FFF00,0xFF8F0000,0xFFFF008F,
	0xFF00408F,0xFF008FC0,0xFF408F00,0xFF8FC000,0xFF8F0040,0xFFC0008F,
	0xFF00808F,0xFF008F80,0xFF808F00,0xFF8F8000,0xFF8F0080,0xFF80008F,
	0xFF00C08F,0xFF008F40,0xFFC08F00,0xFF8F4000,0xFF8F00C0,0xFF40008F,
	0xFF00004F,0xFF004FFF,0xFF004F00,0xFF4FFF00,0xFF4F0000,0xFFFF004F,
	0xFF00404F,0xFF004FC0,0xFF404F00,0xFF4FC000,0xFF4F0040,0xFFC0004F,
	0xFF00804F,0xFF004F80,0xFF804F00,0xFF4F8000,0xFF4F0080,0xFF80004F,
	0xFF00C04F,0xFF004F40,0xFFC04F00,0xFF4F4000,0xFF4F00C0,0xFF40004F,
};
int main(int x,char*y[])
{
	cout<<"<<==<< QuaverStream 12.1 (2020-8-05) >>==>>"<<endl;
	cout<<" > > [ ©2019~2020 Qishipai MIDI Tech ] < < "<<endl;
	cout<<"*****内部优化测试版"<<endl;
	if(x==1)
	{
		cout<<"用法:"<<endl;
		cout<<"\t"<<y[0]<<" <参数1> <参数2> ......"<<endl;
		cout<<"参数列表："<<endl;
		cout<<"\t-mid=  MIDI文件路径     (必须指定)"<<endl;
		cout<<"\t-vid=  视频文件路径     (必须指定)"<<endl;
		cout<<"\t-ppb=  每拍音符长度     (px)(默认520)"<<endl;
		cout<<"\t-fps=  输出视频帧率     (默认60)"<<endl;
		cout<<"\t-wei=  输出视频宽度     (默认1280)"<<endl;
		cout<<"\t-hei=  输出视频高度     (默认720)"<<endl;
		cout<<"\t-keyh= 键盘高度         (默认112)"<<endl;
		cout<<"\t-fthr= ffmpeg滤镜线程数 (默认32)"<<endl;
		cout<<"\t-codr= 编码器           (默认libx264)"<<endl;
		cout<<"声明:"<<endl;
		cout<<"\t本软件完全免费！"<<endl;
		cout<<"\t严禁任何个人或团体用于恰烂钱！"<<endl;
		return 1;
	}
	float ppb=520;
	unsigned int i,j,k,l;
	CANVAS paper;
	MIDI_FILE dat;
	stringstream conv;
	paper.fps=60,paper.keyh=112;
	paper.canvash=720,paper.canvasw=1280;
	string fmidi,video,fthrd("32"),coder("libx264");
	for(i=1;i<x;++i)
	{
		string argd(y[i]),argt;
		if((j=argd.find('='))!=-1)
		{
			argt.assign(y[i],j);
			argd.assign(y[i],j+1,-1);
			for(j=0;j<ARGTC;++j)
				if(argt.compare(argts[j])==0)
					break;
		}
		conv.str(argd);
		switch(j)
		{
		case(0):fmidi.assign(argd);break;
		case(1):video.assign(argd);break;
		case(2):conv>>paper.canvasw;break;
		case(3):conv>>paper.canvash;break;
		case(4):conv>>paper.keyh;break;
		case(5):conv>>paper.fps;break;
		case(6):conv>>ppb;break;
		case(7):fthrd.assign(argd);break;
		case(8):coder.assign(argd);break;
		default:
			cerr<<"[QQS]"<<y[i]<<":无效指令！"<<endl;
			return 1;
		}
		conv.clear();
	}
	if(fmidi.size()==0||video.size()==0)
	{
		cerr<<"[QQS]输入输出文件名需要被指定！"<<endl;
		return 1;
	}
	if(paper.keyh>paper.canvash)
	{
		cerr<<"[QQS]键盘高度不应大于画面高度！"<<endl;
		return 1;
	}
	if(!dat.open(fmidi))
	{
		cerr<<"[QQS]midi文件加载失败！检查路径及格式！"<<endl;
		return 1;
	}
	conv.str("");
	conv<<"-filter_threads "<<fthrd<<" -c:v "<<coder;
	conv<<" \""<<video<<"\"";
	paper.initcanvas(conv.str()),ppb/=dat.ppnq;
	vector<SPD>::iterator spdptr=dat.slist.begin();
	vector<NOTE>::iterator noteptr,notesta=dat.nlist.begin();
	double tick,tickup,spd=(double)dat.ppnq*2/paper.fps;
	for(tick=0;tick<=dat.flength;tick+=spd)
	{
		bool flg=false;
		noteptr=notesta;
		paper.canvasclear();
		tickup=tick+(double)(paper.canvash-paper.keyh)/ppb;
		while(spdptr<dat.slist.end()&&spdptr->pos<=tick)
		{
			spd=(double)1e6/spdptr->val*dat.ppnq/paper.fps;
			++spdptr;
		}
		while(noteptr<dat.nlist.end()&&noteptr->sta<tickup)
		{
			if(noteptr->end>=tick)
			{
				l=color[noteptr->trk%COLORC];
				if(flg?false:flg=true)notesta=noteptr;
				if(noteptr->sta<tick)
				{
					short r=((l&0xFF)+255)>>1;
					short g=(((l&0xFF00)>>8)+255)>>1;
					short b=(((l&0xFF0000)>>16)+255)>>1;
					l=0xFF000000|r|g<<8|b<<16;
					k=paper.keyh;
					j=(float)(noteptr->end-tick)*ppb;
					paper.keycolor[noteptr->key]=l;
				}
				else
				{
					k=(noteptr->sta-tick)*ppb+paper.keyh;
					j=(float)(noteptr->end-noteptr->sta)*ppb;
				}
				if(j+k>paper.canvash)j=paper.canvash-k;
				paper.drawnote(noteptr->key,k,j,l);
			}
			++noteptr;
		}
		paper.drawkeys(),paper.writeframe();
		if(kbhit()&&getch()=='q')
			break;
	}
	paper.canvasdestroy();
	cout<<"[QQS]CHEER!!!!!   乾杯 (￣ε([]~（￣▽￣）~*"<<endl;
	return 0;
}
