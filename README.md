# Трассировка лучей
## Постановка
В пространстве есть некая точка обзора (камера, глаз). Также в пространстве
присутствует некая трёхмёрная конструкция (сцена). Между ними
располагается прямоугольник, поделённый на квадратные пиксели (экран).
Задача трассировки лучей заключается в том, чтобы из точки обзора
провести луч через центр каждого пикселя экрана – и понять, пересекает ли
он какой-либо из объектов сцены. Если луч ничего не пересекает, тогда
соответствующий пиксель экрана нужно закрасить белым цветом. Если луч
пересекает какой-либо из объектов, требуется закрасить пиксель нужным
цветом, отличным от белого. В самом простом варианте этой задачи пиксель
закрашивается чёрным цветом, независимо от того, какой из объектов сцены
он пересекает и в какой точке.
## Объекты на сцене
Существует множество видов трёхмерных геометрических фигур. В нашей
задаче мы ограничимся тремя видами:
 шар (задаётся координатами центра и радиусом);
 бокс – прямоугольный параллелепипед, грани которого параллельны
координатным плоскостям (задаётся координатами двух своих вершин:
той, у которой все три координаты минимальны, и той, у которой все
три координаты максимальны);
 тетраэдр (задаётся координатами четырёх своих вершин).
Самая главная математика в этой задаче будет зашита в функции,
определяющей: пересекается луч с фигурой или нет
## Цвета раскраски
Закрашиваем пиксели разными цветами, в
зависимости от того, какой объект пересекает луч. Учитываем освещенность и глянцевость.
## Сохранение изображения
Программа написана на С++. Это дает возможность распараллеливать на
OpenMP.
Когда всем пикселям экрана задан цвет – его сохраняем в виде BMPизображения. Для этого используется CImg.h.
## Условия
Перспективная проекция, произвольное количество любых фигур. На
экране фигуры отображаются различными оттенками серого цвета, в
зависимости от объёма. Фигуре самого большого объёма соответствует
цвет (64, 64, 64), фигуре самого маленького объёма – цвет (191, 191,
191). Промежуточным фигурам присваивается нечто среднее по
принципу: чем меньше фигура – тем она светлее. Необходим учёт
освещения, все поверхности – глянцевые.
