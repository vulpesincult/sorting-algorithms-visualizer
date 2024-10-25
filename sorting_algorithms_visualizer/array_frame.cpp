#include "array_frame.h"


value_t::value_t() :
	value(0),
	parent(nullptr),
	last_interaction(interactions::none),
	last_interaction_time(std::chrono::steady_clock::now())
{
}

value_t::value_t(size_t value, array_frame * parent) :
	value(value),
	parent(parent),
	last_interaction(interactions::none),
	last_interaction_time(std::chrono::steady_clock::now())
{
}

value_t & value_t::operator=(const value_t & other)
{
	this->value = other.value;
	this->parent = other.parent;

	this->last_interaction = interactions::move;
	other.last_interaction = interactions::move;

	this->last_interaction_time = std::chrono::steady_clock::now();
	other.last_interaction_time = std::chrono::steady_clock::now();

	other.parent->check_interactions();
	other.parent->increment_moves_count();

	return *this;
}

bool value_t::operator<(const value_t & other) const
{
	this->last_interaction = interactions::comparison;
	other.last_interaction = interactions::comparison;

	this->last_interaction_time = std::chrono::steady_clock::now();
	other.last_interaction_time = std::chrono::steady_clock::now();

	other.parent->check_interactions();
	other.parent->increment_comparisons_count();

	return this->value < other.value;
}

bool value_t::operator<=(const value_t & other) const
{
	this->last_interaction = interactions::comparison;
	other.last_interaction = interactions::comparison;

	this->last_interaction_time = std::chrono::steady_clock::now();
	other.last_interaction_time = std::chrono::steady_clock::now();

	other.parent->check_interactions();
	other.parent->increment_comparisons_count();

	return this->value <= other.value;
}

bool value_t::operator>(const value_t & other) const
{
	this->last_interaction = interactions::comparison;
	other.last_interaction = interactions::comparison;

	this->last_interaction_time = std::chrono::steady_clock::now();
	other.last_interaction_time = std::chrono::steady_clock::now();

	other.parent->check_interactions();
	other.parent->increment_comparisons_count();

	return this->value > other.value;
}

bool value_t::operator>=(const value_t & other) const
{
	this->last_interaction = interactions::comparison;
	other.last_interaction = interactions::comparison;

	this->last_interaction_time = std::chrono::steady_clock::now();
	other.last_interaction_time = std::chrono::steady_clock::now();

	other.parent->check_interactions();
	other.parent->increment_comparisons_count();

	return this->value >= other.value;
}

bool value_t::operator==(const value_t & other) const
{
	this->last_interaction = interactions::comparison;
	other.last_interaction = interactions::comparison;

	this->last_interaction_time = std::chrono::steady_clock::now();
	other.last_interaction_time = std::chrono::steady_clock::now();

	other.parent->check_interactions();
	other.parent->increment_comparisons_count();

	return this->value == other.value;
}

bool value_t::operator!=(const value_t & other) const
{
	this->last_interaction = interactions::comparison;
	other.last_interaction = interactions::comparison;

	this->last_interaction_time = std::chrono::steady_clock::now();
	other.last_interaction_time = std::chrono::steady_clock::now();

	other.parent->check_interactions();
	other.parent->increment_comparisons_count();

	return this->value != other.value;
}

size_t value_t::operator>>(size_t offset) const
{
	this->last_interaction = interactions::shift;

	this->last_interaction_time = std::chrono::steady_clock::now();

	this->parent->check_interactions();
	this->parent->increment_shifts_count();

	return this->value >> offset;
}

size_t value_t::operator<<(size_t offset) const
{
	this->last_interaction = interactions::shift;

	this->last_interaction_time = std::chrono::steady_clock::now();

	this->parent->check_interactions();
	this->parent->increment_shifts_count();

	return this->value << offset;
}

value_t value_t::operator-(const value_t & other) const
{
	return value_t(this->value - other.value, parent);
}

value_t::operator uint64_t() const
{
	return value;
}




void array_frame::initialize()
{
	QPalette palette;
	palette.setColor(QPalette::Window, Qt::black);
	palette.setBrush(QPalette::Window, QBrush(Qt::black));
	this->setAutoFillBackground(true);
	this->setPalette(palette);
	info_label.setObjectName(QString::fromUtf8("info_label"));
	info_label.setGeometry(QRect(10, 0, 500, 15));
	info_label.setStyleSheet("color: rgb(255, 255, 255)");

	update();
}

array_frame::array_frame(QWidget * parent = Q_NULLPTR) :
	max_value(default_array_size),
	values(default_array_size),
	painter(this),
	info_label(this),
	delay(0),
	is_paused(false),
	is_steping(false),
	is_breaking_operations(false)
{
	initialize();
}

array_frame::array_frame(size_t array_size, QWidget * parent = Q_NULLPTR) :
	max_value(array_size),
	values(array_size),
	painter(this),
	info_label(this),
	delay(0),
	is_paused(false),
	is_steping(false),
	is_breaking_operations(false)
{
	initialize();
}

array_frame::~array_frame()
{
}

void array_frame::make_step()
{
	if (is_paused == false)
	{
		is_paused = true;
		return;
	}

	is_steping = true;
	is_paused = false;
}

void array_frame::break_operations()
{
	is_breaking_operations = true;
}

void array_frame::paintEvent(QPaintEvent * event)
{
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	painter.begin(this);

	size_t width = this->width() - horizontal_offset * 2;
	size_t height = this->height() - vertical_offset * 2;
	float step = std::max(float(values.size()) / width, float(1));
	float bars_width = (float)width / values.size();
	if (bars_width < 1)
	{
		bars_width = 1;
	}

	int num_bars = std::min({ width, values.size() });
	std::vector<int> bars_heights(num_bars);
	std::vector<uint32_t> bars_colors(num_bars);

	for (int i = 0;
		i != num_bars;
		++i)
	{
		bars_heights[i] = values[int(step * i)].value;
		if ((std::chrono::steady_clock::now() - values[int(step * i)].last_interaction_time).count() < highlight_ttl)
		{
			bars_colors[i] = highlight_colors.at(values[int(step * i)].last_interaction);
		}
		else
		{
			bars_colors[i] = colors::white;
		}
	}

	uint32_t last_color = colors::white;

	std::vector<QLine> lines(num_bars * 2 + 2);
	int line_index = 0;


	for (int i = 0;
		;
		++i)
	{
		if (i == num_bars or last_color != bars_colors[i])
		{
			painter.setPen(QPen(last_color));
			painter.setBrush(QBrush(last_color));

			painter.drawLines(lines.data(), line_index);

			line_index = 0;

			if (i == num_bars)
			{
				break;
			}

			last_color = bars_colors[i];
		}

		lines[line_index] = QLine(horizontal_offset + i * bars_width, height, horizontal_offset + i * bars_width, height - bars_heights[i] * height / max_value);
		line_index++;
	}

	painter.end();

	int64_t runtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

	QString moves_counter_str = QString::number(moves_count);
	for (int i = moves_counter_str.size() - 3; i >= 0; i -= 3)
	{
		moves_counter_str.insert(i, '`');
	}
	QString compares_count_str = QString::number(comparisons_count);
	for (int i = compares_count_str.size() - 3; i >= 0; i -= 3)
	{
		compares_count_str.insert(i, '`');
	}
	QString shifts_count_str = QString::number(shifts_count);
	for (int i = shifts_count_str.size() - 3; i >= 0; i -= 3)
	{
		shifts_count_str.insert(i, '`');
	}
	info_label.setText(/*QString::number(1000 / (runtime + 1)) + */"; moves: " + moves_counter_str + "; comparisons: " + compares_count_str + "; shifts: " + shifts_count_str);

	update();
}

void array_frame::timerEvent(QTimerEvent * event)
{
	update();
}

typename array_frame::iterator array_frame::get_array_begin()
{
	return values.begin();
}

typename array_frame::iterator array_frame::get_array_end()
{
	return values.end();
}

size_t array_frame::get_array_size() const
{
	return values.size();
}

void array_frame::change_size(size_t new_size)
{
	values.resize(new_size);
	max_value = new_size;
	for (array_frame::iterator i = get_array_begin();
		i < get_array_end();
		++i)
	{
		i->last_interaction = interactions::none;
		i->parent = this;
		i->value = ((i - get_array_begin()) % get_array_size());
	}
}

void array_frame::reset_counters()
{
	moves_count = 0;
	comparisons_count = 0;
	shifts_count = 0;
}

void array_frame::check_interactions()
{
	if (is_breaking_operations)
	{
		is_breaking_operations = false;
		throw exit_excep();
	}

	if (is_steping)
	{
		is_steping = false;
		is_paused = true;
	}
	else
	{
		std::chrono::steady_clock::time_point dieTime = std::chrono::steady_clock::now() + delay;
		while (std::chrono::steady_clock::now() < dieTime)
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}

	while (is_paused) {}
}


void array_frame::increment_moves_count()
{
	++moves_count;
}

void array_frame::increment_comparisons_count()
{
	++comparisons_count;
}

void array_frame::increment_shifts_count()
{
	++shifts_count;
}

void array_frame::set_delay(std::chrono::nanoseconds new_delay)
{
	delay = new_delay;
}

void array_frame::switch_pause_operations()
{
	is_paused = !is_paused;
}

void array_frame::unpause_operations()
{
	is_paused = false;
}