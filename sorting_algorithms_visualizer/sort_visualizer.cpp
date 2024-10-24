#include "sort_visualizer.h"



QString to_str(const algorithms & algorithm)
{
	return algo_name_map[algorithm];
}

algorithms get_algorithm(const QString & name)
{
	for (auto algo_name_pair = algo_name_map.keyValueBegin();
		algo_name_pair != algo_name_map.keyValueEnd();
		++algo_name_pair)
	{
		if ((*algo_name_pair).second == name)
		{
			return (*algo_name_pair).first;
		}
	}

	return algorithms::std_sort;
}

void sort_visualizer::fill_array(size_t new_size)
{
	ui.frame->change_size(new_size);
}

sort_visualizer::sort_visualizer(QWidget * parent) :
	QMainWindow(parent),
	threads(1)
{
	ui.setupUi(this);
	current_algorithm = algorithms::std_sort;

	ui.algorithms_list_widget->addItems(QStringList(algo_name_map.values()));

	ui.frame->set_delay(std::chrono::nanoseconds(0));
	ui.delay_spin_box->setValue(0);
	ui.frame->reset_counters();

	ui.array_size_spin_box->setValue(ui.frame->get_array_size());

	is_running = false;
}

sort_visualizer::~sort_visualizer()
{
	ui.frame->break_operations();
	ui.frame->unpause_operations();
	ui.frame->set_delay(std::chrono::nanoseconds(0));
}

void sort_visualizer::on_shuffle_button_clicked()
{
	if (is_running)
	{
		return;
	}

	threads.add_task([&]
	{
		is_running = true;
		ui.frame->reset_counters();
		try
		{
			sorts::my_shuffle(ui.frame->get_array_begin(), ui.frame->get_array_end());
		}
		catch (exit_excep)
		{
			is_running = false;
		}
		is_running = false;
	});
}

void sort_visualizer::on_sort_button_clicked()
{
	if (is_running)
	{
		return;
	}

	threads.add_task([&]
	{
		is_running = true;
		ui.frame->reset_counters();
		try
		{
			switch (current_algorithm)
			{
			case algorithms::std_sort:
				std::sort(ui.frame->get_array_begin(), ui.frame->get_array_end());
				break;
			case algorithms::std_stable_sort:
				std::stable_sort(ui.frame->get_array_begin(), ui.frame->get_array_end());
				break;
			case algorithms::std_heap_sort:
				std::make_heap(ui.frame->get_array_begin(), ui.frame->get_array_end());
				std::sort_heap(ui.frame->get_array_begin(), ui.frame->get_array_end());
				break;

#if __has_include(<boost/sort/sort.hpp>)
			case algorithms::boost_pdqsort:
				boost::sort::pdqsort(ui.frame->get_array_begin(), ui.frame->get_array_end());
				break;
			case algorithms::boost_pdqsort_branchless:
				boost::sort::pdqsort_branchless(ui.frame->get_array_begin(), ui.frame->get_array_end());
				break;
			case algorithms::boost_integer_sort:
				boost::sort::spreadsort::integer_sort(ui.frame->get_array_begin(), ui.frame->get_array_end(), [](const value_t & i, const size_t & j) { return i >> j; });
				break;
#endif

			case algorithms::dual_pivot_quick_sort:
				sorts::dual_pivot_qs(ui.frame->get_array_begin(), ui.frame->get_array_end());
				break;
			case algorithms::d_heap_sort:
				sorts::heap_sort(ui.frame->get_array_begin(), ui.frame->get_array_end(), 4, std::less<>());
				break;
			case algorithms::shell_sort:
				sorts::shell_sort(ui.frame->get_array_begin(), ui.frame->get_array_end(), std::less<>());
				break;
			case algorithms::timsort:
				sorts::timsort(ui.frame->get_array_begin(), ui.frame->get_array_end(), std::less<>());
				break;
			default:
				break;
			}
		}
		catch (exit_excep)
		{
			is_running = false;
		}
		is_running = false;
	});
}

void sort_visualizer::on_pause_button_clicked()
{
	ui.frame->switch_pause_operations();
}

void sort_visualizer::on_step_button_clicked()
{
	ui.frame->make_step();
}

void sort_visualizer::on_break_button_clicked()
{
	if (is_running)
	{
		ui.frame->unpause_operations();
		ui.frame->break_operations();
		threads.cancel_pending();
	}
}

void sort_visualizer::on_delay_spin_box_valueChanged(int value)
{
	ui.frame->set_delay(std::chrono::nanoseconds(value));
}

void sort_visualizer::on_array_size_spin_box_valueChanged(int value)
{
	if (is_running)
	{
		ui.array_size_spin_box->setValue(ui.frame->get_array_size());
		return;
	}

	fill_array(value);
}

void sort_visualizer::on_algorithms_list_widget_currentItemChanged(QListWidgetItem * new_item, QListWidgetItem * old_item)
{
	current_algorithm = get_algorithm(new_item->text());
}