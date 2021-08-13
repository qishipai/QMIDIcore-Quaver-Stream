#include<iostream>
#include<iomanip>
#include<queue>
#include<algorithm>
#include"quaverstream.h"
using namespace std;
using namespace QQS;
QQS::SPD::SPD(const char*a,unsigned int b)
{
	val=revu32(*(unsigned int*)a)>>8,pos=b;
}
bool QQS::SPD::operator<(const SPD&B)const
{
	return pos<B.pos;
}
QQS::NOTE::NOTE(short a,short b,unsigned int c)
{
	key=a,trk=b,sta=c;
}
bool QQS::NOTE::operator<(const NOTE&B)const
{
	return sta<B.sta||(sta==B.sta&&trk<B.trk);
}
unsigned int QQS::revu32(unsigned int x)
{
	return x<<24|(x&0xFF00)<<8|(x&0xFF0000)>>8|x>>24;
}
unsigned int MIDI_FILE::gvln(short x)
{
	unsigned int y=x&0x7F;
	while(x&0x80)
	{
		fread(&x,1,1,midifp);
		(y<<=7)|=x&0x7F;
	}
	return y;
}
bool MIDI_FILE::open(string fname)
{
	short t=0;
	unsigned int tmp=0;
	queue<int>stat[128];
	midifp=fopen(fname.c_str(),"rb");
	if(!midifp)
	{
		cerr<<"[MIDIloader]指定MIDI文件不存在！"<<endl;
		return false;
	}
	fread(&tmp,4,1,midifp);
	if(tmp!=0x6468544Du)
	{
		cerr<<"[MIDIreader]指定输入不是MIDI文件！"<<endl;
		fclose(midifp);
		return false;
	}
	fread(&tmp,4,1,midifp);
	trkend=ftello(midifp)+revu32(tmp);
	fread(&(tmp=0),2,1,midifp);
	if((tmp<<8|tmp>>8)==2)
	{
		cerr<<"[MIDIreader]不支持的MIDI文件类型！"<<endl;
		fclose(midifp);
		return false;
	}
	fread(&tmp,2,1,midifp);
	trks=tmp<<8|tmp>>8;
	fread(&tmp,2,1,midifp);
	ppnq=tmp<<8|tmp>>8;
	for(flength=0;t<trks;++t)
	{
		tmp=abstick=typ=0;
		fseeko(midifp,trkend,SEEK_SET);
		fread(&tmp,4,1,midifp);
		if(tmp!=0x6B72544Du)
		{
			cerr<<"[MIDIreader]MIDI轨道头信息错误！"<<endl;
			fclose(midifp);
			return false;
		}
		bool act=true;
		fread(&tmp,4,1,midifp);
		trkend=ftello(midifp)+revu32(tmp);
		while(ftello(midifp)<trkend&&act)
		{
			//unsigned char chan=0;
			abstick+=gvln(0x80);
			fread(&(tmp=0),1,1,midifp);
			if(tmp&0x80)
			{
				//chan=tmp&0x0F;
				typ=tmp==0xFF?0xFF:tmp&0xF0;
				fread(&tmp,1,1,midifp);
			}
			switch(typ)
			{
			case(0x80):case(0x90):
			case(0xA0):case(0xB0):
				num=tmp,val=0;
				fread(&val,1,1,midifp);
				break;
			case(0xC0):case(0xD0):
				val=tmp;
				break;
			case(0xE0):
				val=tmp<<7;
				fread(&tmp,1,1,midifp);
				val|=tmp;
				break;
			case(0xFF):
				num=tmp;
				fread(&tmp,1,1,midifp);
			case(0xF0):
				edata=new char[edsize=gvln(tmp)];
				fread(edata,edsize,1,midifp);
				break;
			default:
				cerr<<"[MIDIreader]发现无法解释的MIDI事件！"<<endl;
				fclose(midifp);
				return false;
			}
			switch(typ)
			{
			case(0xFF):
				if(num==0x51)
					slist.push_back(SPD(edata,abstick));
				if(num==0x2F)
					act=false;
				break;
			case(0x90):
				if (val == 0)
					goto __noteoff;
				stat[num].push(nlist.size());
				nlist.push_back(NOTE(num,t,abstick));
				break;
			case(0x80):
			__noteoff:
				if(stat[num].empty())
				{
					cerr<<"[MIDIloader]警告！发现多余的noteoff事件！";
					cerr<<"(key="<<num<<",track="<<t<<")"<<endl;
				}
				else
				{
					nlist[stat[num].front()].end=abstick;
					stat[num].pop();
				}
			}
			if((typ&0xF0)==0xF0)
				delete[]edata;
		}
		if(abstick>flength)
			flength=abstick;
		for(int i=0;i<128;++i)
			while(!stat[i].empty())
			{
				cerr<<"[MIDIloader]警告！发现多余的noteon事件！";
				cerr<<"(key="<<i<<",track="<<t<<")"<<endl;
				nlist[stat[i].front()].end=nlist[stat[i].front()].sta;
				stat[i].pop();
			}
		cout<<"[MIDIloader]已加载"<<setw(4)<<t+1<<"音轨";
		cout<<setfill('~')<<setw(10)<<nlist.size()<<"音符\r";
		(cout<<setfill(' ')).flush();
	}
	fclose(midifp);
	sort(slist.begin(),slist.end());
	sort(nlist.begin(),nlist.end());
	cout<<endl<<"[MIDIloader]MIDI加载完毕！！！"<<endl;
	cout<<"[MIDIloader]轨道数："<<setw(9)<<trks<<endl;
	cout<<"[MIDIloader]分辨率："<<setw(9)<<ppnq<<endl;
	cout<<"[MIDIloader]音符数："<<setw(9)<<nlist.size()<<endl;
	return true;
}
