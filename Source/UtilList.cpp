#include "utils.h"

SmoothList::SmoothList(int num, double k): num(num), k(0.9)
{
	setN(num);
}

SmoothList::~SmoothList()
{
}

void SmoothList::setN(int num)
{
	this->num = num;
	list.resize(num);
	for (int i = 0; i < num; i++) {
		list[i] = Smooth(k=this->k);
	}
}

void SmoothList::addSamples(std::vector<double> data)
{
	for (int i = 0; i < num; i++) {
		list[i].addSample(data[i]);
	}
}

std::vector<double> SmoothList::getRes()
{
	std::vector<double> tmp(num);
	for (int i = 0; i < num; i++) {
		tmp[i] = list[i].getRes();
	}
	return tmp;
}

void SmoothList::clear()
{
	for (Smooth smooth : list) {
		smooth.clear();
	}
}

void SmoothList::setK(double k) {
	this->k = k;
	for (Smooth smooth : list) {
		smooth.setK(k);
	}
}



STDList::STDList(int num) : num(num)
{
	setN(num);
}

STDList::~STDList()
{
}

void STDList::setN(int num)
{
	this->num = num;
	list.resize(num);
	for (int i = 0; i < num; i++) {
		list[i] = RealtimeSTD();
	}
}

void STDList::addSamples(std::vector<double> data)
{
	for (int i = 0; i < num; i++) {
		list[i].addSample(data[i]);
	}
}

void STDList::clear()
{
	for (RealtimeSTD std : list) {
		std.clear();
	}
}
std::vector<double> STDList::getResN(float n)
{
	std::vector<double> tmp(num);
	for (int i = 0; i < num; i++) {
		tmp[i] = list[i].getResN(n);
	}
	return tmp;
}