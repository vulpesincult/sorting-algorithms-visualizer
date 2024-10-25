#pragma once

#include <qframe.h>
#include <qpainter.h>
#include <thread>
#include <future>
#include <chrono>
#include <QtWidgets/QLabel>
#include <fstream>
#include <map>
#include <QTime>
#include <QEventLoop>
#include <QCoreApplication>
#include <QFrame>

enum colors
{
	blue = 255,
	green = 255 * 256,
	red = 255 * 256 * 256,
	black = 0,
	azure = green + blue,
	violet = red + blue,
	yellow = red + green,
	white = red + green + blue,
};

enum interactions
{
	comparison,
	move,
	shift,
	none,
};

static const std::map<interactions, colors> highlight_colors = {
	{ comparison, green },
	{ move, red },
	{ shift, blue },
	{ none, white }
};

struct exit_excep : public std::exception
{
	exit_excep() {}
};

struct value_t;

class array_frame : public QFrame
{
public:

	typedef std::vector<value_t>::iterator iterator;
	typedef value_t element;

	array_frame(QWidget * parent);
	array_frame(size_t array_size, QWidget * parent);
	~array_frame();

	iterator get_array_begin();
	iterator get_array_end();
	size_t get_array_size() const;
	void change_size(size_t new_size);

	void make_step();
	void break_operations();
	void reset_counters();
	void check_interactions();
	void increment_moves_count();
	void increment_comparisons_count();
	void increment_shifts_count();
	void set_delay(std::chrono::nanoseconds new_delay);
	void switch_pause_operations();
	void unpause_operations();


private:

	std::vector<value_t> values;

	uint64_t max_value;
	uint64_t moves_count, comparisons_count, shifts_count;
	std::chrono::nanoseconds delay;
	std::atomic<bool> is_steping;
	std::atomic<bool> is_paused;
	std::atomic<bool> is_breaking_operations;

	static const size_t highlight_ttl = 10000000;
	static const size_t default_array_size = 1000;
	static const size_t horizontal_offset = 5;
	static const size_t vertical_offset = 5;

	QLabel info_label;
	QPainter painter;

	void initialize();
	void paintEvent(QPaintEvent * event);
	void timerEvent(QTimerEvent * event);
};

struct value_t
{
	value_t();
	value_t(size_t _value, array_frame * parent);
	value_t & operator=(const value_t & other);
	bool operator<(const value_t & other) const;
	bool operator<=(const value_t & other) const;
	bool operator>(const value_t & other) const;
	bool operator>=(const value_t & other) const;
	bool operator==(const value_t & other) const;
	bool operator!=(const value_t & other) const;
	size_t operator>>(const size_t offset) const;
	size_t operator<<(const size_t offset) const;
	value_t operator-(const value_t & other) const;
	operator uint64_t() const;

	uint64_t value;
	mutable array_frame * parent;
	mutable interactions last_interaction;
	mutable std::chrono::steady_clock::time_point last_interaction_time;
};
