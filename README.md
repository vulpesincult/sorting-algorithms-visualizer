﻿﻿# sorting algorithms visualizer

Приложение для визуализации алгоритмов сортировки. В отличии от других схожих проектов, не требует переписывать алгоритмы, позволяя напрямую визуализировать реализации из std и boost.

Пример визуализаци boost::pdqsort на массиве из 10000 элементов:

![boost:pdqsort](https://github.com/vulpesincult/sorting-algorithms-visualizer/blob/main/boost_pdqsort_animation.gif)

Работает это следующим образом:

Объект array_frame содержит в себе массив значений которые отрисовываются по вызову paintEvent. 

При нажатии кнопки sort вызывается функция сортировки которой передаются итераторы начала и конца массива из array_frame.

Когда функция сортировки обращается к операторам сравнения/присваивания объектов массива, они запоминают это и при следующем отрисовывании отображаются в разных цветах в зависимости от того что делает с ними алгоритм сотрировки. 

Сортировка происходит в отдельном потоке, поэтому основной поток может отрисовывать изменения в массиве в реальном времени.
