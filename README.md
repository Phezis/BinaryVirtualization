# Binary Virtualization
---

Данное решение представляет из себя три шаблонных класса: 
- VirtualPointer
- BinaryReader 
- BinaryWriter

Класс VirtualPointer независимый. Исходный код находится в одноименном проекте.

Классы BinaryReader и BinaryWriter зависят от VirtualPointer. 
Исходный код находится в проекте BinaryRW.

Для каждого класса написаны тесты. Для каждого класса написаны тесты.
Исходный код тестов находится в проектах с постфиксом Test:
- VirtualPointerTest
- BinaryRWTest 

Дополнительно написаты тесты производительности для виртуального указателя.
Исходный код тестов находится в проекте VirtualPointerPerformanceTest.