# TableEditor

Приложение для работы с электронными таблицами с базовым функционалом.

## Функционал

- Добавление и удаление строк и колонок.
- Поддержка типов данных колонок: `String`, `Number`, `Date`, `Boolean`.
- Сохранение и загрузка таблиц в формате Microsoft Excel 2003 XML (XML Spreadsheet).
- Натуральная сортировка по содержимому ячеек.

## Используемые технологии

- C++17
- Qt5 (модули Widgets, Core, Xml)
- CMake (версия не ниже 3.5)

## Структура проекта

- `main.cpp` — точка входа в программу.
- `mainwindow.*` — реализация главного окна приложения.
- `tablemodel.*` — собственная модель данных таблицы.
- `delegates/` — делегаты для редактирования разных типов данных.
- `parser/` — парсер XML формата Excel.
- `serializer/` — сохранение данных в XML.
- `columnTypeUtils.h` — утилиты для работы с типами колонок.
- `NaturelSort.h` — реализация натуральной сортировки.

## Сборка

### Требования

- Qt5 (с установленными модулями Widgets, Core, Xml)
- CMake 3.5 или выше
- Компилятор с поддержкой C++17 (GCC, Clang, MSVC)

### Инструкция по сборке (Linux)

```bash
git clone https://github.com/KolyanPWNZ/tableEditor.git
cd tableEditor
mkdir build
cd build
cmake ..
make
./tableEditor
```

## 
