# Binary Virtualization

Данное решение представляет из себя три шаблонных класса: 
- VirtualPointer
- BinaryReader 
- BinaryWriter

Класс VirtualPointer независимый. Исходный код находится в одноименном проекте.

Классы BinaryReader и BinaryWriter зависят от VirtualPointer. 
Исходный код находится в проекте BinaryRW.

Для каждого класса написаны тесты корректности и примеры использования.
Исходный код тестов находится в проектах с постфиксом Test:
- VirtualPointerTest
- BinaryRWTest 

Исходный код примеров находится в проектах с постфиксом Example в директории Examples:
- BinaryReaderExample
- BinaryWriterExample
- VirtualPointerExample

Дополнительно написаты тесты производительности для виртуального указателя.
Исходный код тестов находится в проекте VirtualPointerPerformanceTest.

Более подробное описание классов находится в директории doc