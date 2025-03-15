#include "utils.h"

SlidingWindow::SlidingWindow() : window_size_(600), sum_(0.0), index_(0)
{
	setWindowSize(window_size_);
}

double SlidingWindow::addSample(double in_sample)
{
	sum_ += ((in_sample)-buffer_[index_]);
	buffer_[index_] = in_sample;
	index_ = (index_ + 1) % window_size_;
	return in_sample - sum_ / window_size_;
}

void SlidingWindow::setWindowSize(size_t window_size)
{
	buffer_.resize(window_size_, 0.0);
	window_size_ = window_size;
	clear();
}

void SlidingWindow::clear()
{
	sum_ = 0;
	index_ = 0;
}
