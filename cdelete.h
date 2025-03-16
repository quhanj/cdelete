#ifndef CDELETE_H
#define CDELETE_H
enum cdeletetype{
	CDBUF=1,CDOPEN,CDFOP,CDJSON,CDPQC,CDPQR,CDCURL
};
#define cdelete(p,t) _cdelete((void*)&(p),(t))
void _cdelete(void** p,enum cdeletetype t);
#endif