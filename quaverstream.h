#ifndef QUAVERSTREAM
#define QUAVERSTREAM
#include<cstdio>
#include<vector>
#include<string>
namespace QQS
{
	class SPD
	{
	public:
		int val;
		unsigned int pos;
		SPD(const char*,unsigned int);
		bool operator<(const SPD&)const;
	};
	class NOTE
	{
	public:
		short key,trk;
		unsigned int sta,end;
		NOTE(short,short,unsigned int);
		bool operator<(const NOTE&)const;
	};
	unsigned int revu32(unsigned int);
};
class MIDI_FILE
{
private:
	FILE*midifp;
	char*edata;
	off_t trkend;
	unsigned short typ,num,val;
	unsigned int edsize,abstick;
	unsigned int gvln(short);
public:
	short trks,ppnq;
	unsigned int flength;
	std::vector<QQS::SPD>slist;
	std::vector<QQS::NOTE>nlist;
	bool open(std::string);
};
class CANVAS
{
public:
	int canvasw,canvash,keyh,fps;
	unsigned int keycolor[128];
	void initcanvas(std::string);
	void canvasdestroy();
	void canvasclear();
	void writeframe()const;
	void drawkeys();
	void drawnote(short,int,int,unsigned int);
private:
	FILE*ffpipe;
	unsigned int*candat;
	int keyx[128],keyw[128];
	void rect(int,int,int,int,unsigned int);
	void fillrect(int,int,int,int,unsigned int);
};
#endif /*ifndef QUAVERSTREAM*/
