# Binary Writer
---

## Вкратце о классе
---
ВАЖНО! На текущий момент библиотека работоспособна только при работе с типом T размера 1 байт.

BinaryWriter предоставляет возможность писать произвольное количество бит в участок памяти, но не больше, чем вмещает в себя машинное слово. 
Класс умеет учитывать порядок байт в слове и порядок бит в байте - ***little endian*** и ***big endian***. Ответственность за выбор режима лежит на пользователе - нужно вручную указать, нужно ли менять порядок бит и/или байт исходя из типа обрабатываемой информации. Например, если код компилируется для работы на little endian машине с данными для отправки по сети, которые должны быть записаны в big endian формате, то необходимо выставить режим работы с заменой порядка байт в слове.
Перед началом работы объекту класса необходимо передать указатель на начало области памяти, в которую будет производиться запись, и ее размер. Важно, что размер этой области не должен превышать (size_t::max / 8) байт.
Переданный участок памяти должен быть действительным на протяжении всей жизни объекта.
После инициализации можно производить запись. Записывающие операции принимают количество бит, которые необходимо записать и переменную, биты которой необходимо записать. Записываются младшие биты. 
Запись производится последовательно, вернуться к записанным битам нельзя. Инициализация записи не обязательно сразу изменит содержимое памяти. Чтобы гарантировать, что запись была произведена, нужно вызвать специальную функцию flush.

## Интерфейс
---
### Пользовательское определение типов
Для класса определены следующие типы:
* using reverse_bytes_t = bool;
* using reverse_bits_t = bool;

Для данных типов определены константы:
* constexpr reverse_bytes_t REVERSE_BYTES = true;
* constexpr reverse_bits_t REVERSE_BITS = true;

Для обратного эффекта использовать !REVERSE_*

### Конструктор
	BinaryWriter();                                                             (1)
	explicit BinaryWriter(reverse_bytes_t reverseBytes);                        (2)
	BinaryWriter(reverse_bytes_t reverseBytes, reverse_bits_t reverseBits);     (3)

1) Создает неинициализированный обрабатываемой памятью объект, в режиме работы с обратным порядком байт в слове и с сохранением порядка бит в байте.
2) Создает неинициализированный обрабатываемой памятью объект, в режиме работы с пользовательским порядком байт в слове и с сохранением порядка бит в байте.
3) Создает неинициализированный обрабатываемой памятью объект, в режиме работы с пользовательским порядком байт в слове и  с пользовательским порядком бит в байте.

### Деструктор
	~BinaryWriter() noexcept;       (1)
	
1) Определен стандартный деструктор.

### Выбор режима чтения
	void setReverseBits(reverse_bits_t val);        (1)
	void setReverseBytes(reverse_bytes_t val);      (2)
	
1) Метод устанавливает режим перестановки бит в val. После исполнения метода до следующего вызова setData поведение не определено. При значении, установленном в REVERSE_BITS, во время записи биты в байтах, записываемых в заданную область, будут переставляться.
2) Метод устанавливает режим перестановки байт в val. После исполнения метода до следующего вызова setData поведение не определено. При значении, установленном в REVERSE_BYTES, область интерпретируется как область с порядком байт, противоположным порядку текущей архитектуры – во время записи машинного слова в область памяти байты в нем будут переставляться.

### Выбор обрабатываемой памяти
	void setData(T* address, std::size_t sizeInBytes);                    (1)
	void setData(VirtualPointer<T>& address, std::size_t sizeInBytes);    (2)
    void setData(VirtualPointer<T>& address);                             (3)

1) Запоминает переданный фрагмент и его размер. Предыдущий фрагмент забывается. Значение sizeInBytes не должно превышать std::size_t::max / 8.
2) Запоминает переданный фрагмент в виде виртуального указателя и его размер. Предыдущий фрагмент забывается. Параметр sizeInBytes может быть больше, чем суммарный размер в байтах всех фрагментов address, тогда поведение при попытке записать данные сверх реального размера не определено. Значение sizeInBytes не должно превышать std::size_t::max / 8.
3) Аналогичен (2) с наибольшим по велечине допустимым вторым параметром. Подразумевает, что суммарный размер фрагментов address может увеличиваться по ходу работы программы.

### Запись

	bool writeBits(std::size_t count, std::size_t value);               (1)
	void flush();                                                       (2)

1) Метод буферизированно записыват count младших бит value в переданную область памяти. Последующа запись продолжится с того бита, который следовал за последним записанным. При попытке записать больше бит, чем осталось, не выполняет запись и возвращает false. Тип value должен быть интегральным, иначе поведение не определено. Если count не положительный или больше, чем чем размер машинного слова в битах, то поведение не определено. 
2) Записывает все буферизированные данные в память. Содержимое битов элементов типа T в памяти, которые не были записаны, не определено.

### Дополнительные методы

	std::size_t getRemainSize() const;                      (1)

1. Возвращает количество доступных для записи бит. 

## Потокобезопасность
---
Класс не является потокобезопасным.

## Использование
---
### Подключение
Для использования библиотеки необходимо использовать (добавить в проект) следующие заголовочные файлы:
- Exceptions.h
- VirtualPointer.h
- BitMask.h
- Reverser.h

А также статическую библиотеку BinaryRW.lib.

### Пример использования
    #include <cstdlib>
    #include <cstddef>
    #include <iostream>
    #include <BinaryWriter.h>
    #include <VirtualPointer.h>
    
    using std::size_t;
    
    int main()
    {
    	// Test is correct if it runs on the little endian architecture
    	constexpr auto chunkSize = 5;
    	uint8_t chunk[chunkSize] = { 0 };
    	BinaryWriter<uint8_t> writer(REVERSE_BYTES, !REVERSE_BITS);
    	writer.setData(chunk, chunkSize);
    
    	size_t storage = 0x00;
    	writer.writeBits(8, storage);
    
    	storage = 0x0F;
    	writer.writeBits(4, storage);
    	writer.writeBits(2, storage);
    	writer.flush();
    	// Second byte == 111111xx; x - undefined bit
		std::cout << "After flush 6 bits of the second byte is 0x" << std::hex
			<< static_cast<int>(chunk[1] >> 2) << std::dec << std::endl;
    	writer.writeBits(1, storage);
    	writer.writeBits(1, storage);
    	writer.flush();
    	std::cout << "After filling 2 bytes out of 5, " << writer.getRemainSize() << " bits remain" << std::endl;
    	std::cout << "First byte was written as " << static_cast<int>(chunk[0]) << std::endl;
    	std::cout << "Second byte was written as " << static_cast<int>(chunk[1]) << std::endl;
    
    	VirtualPointer<uint8_t> vChunk{};
    	vChunk.addChunk(chunk + 2, chunkSize - 2);
    
    	writer.setData(vChunk, chunkSize - 2);
    	storage = 0x030405;
    	writer.writeBits(24, storage);
    	
    	std::cout << "Last 3 bytes were written as they are indexed: ";
    	for(auto vptr = vChunk; vptr.bytesRemaining(); ++vptr)
    	{
    		std::cout << static_cast<int>(*vptr) << "; ";
    	}
    	std::cout << std::endl;
    	return EXIT_SUCCESS;
    }


Вывод:

    After flush 6 bits of the second byte is 0x3f
    After filling 2 bytes out of 5, 24 bits remain
    First byte was written as 0
    Second byte was written as 255
    Last 3 bytes were written as they are indexed: 3; 4; 5;
