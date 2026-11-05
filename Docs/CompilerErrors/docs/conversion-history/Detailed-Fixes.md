> **Исторический журнал.** Этот файл сохранён из прежнего README. Упоминаемые в нём имена файлов относятся к старой плоской структуре: инструменты теперь находятся в `../../tools/`, заметки и снимки — в соседних папках `../`, а `../../../legacy/` теперь содержит исходный подкаталог `2\Legacy`; ссылки на старые расположения compiler logs в последующих записях являются историческими. Скрипты исторические: перед запуском проверьте абсолютные пути и параметры сборки.

# Исправления ошибок компиляции

## Порядок const и override

Конвертер сформировал объявления `... override const;`, что недопустимо в C++. В 14 объявлениях заменено на `... const override;`, без изменения константности или требования переопределения. Скрипт `fix-override-const.ps1` по умолчанию показывает совпадения; параметр `-Apply` применяет исправление. Повторный поиск: 0 совпадений.

Проверка: MSVC 14.39, `cl /nologo /std:c++20 /Zs override-const-check.cpp`. До исправления контрольный пример выдавал C2143 и C3668, после — код возврата 0. Это проверка синтаксиса, а не подтверждение сборки проекта: остаются ошибки конвертации базовых классов и других типов.

## Циклическое включение SlotClass

`SlotClassDefinitions.h` использует `MicroOp` только через `shared_ptr`; вместо включения `MicroOp.h` добавлено предварительное объявление, а включение перенесено в `.cpp`. Удалён некорректный разделитель сразу после `0b` в восьми бинарных литералах. Изолированная синтаксическая компиляция `SlotPlacementMetadata.h` с предварительным объявлением незавершённого `ReadOnlySpan` прошла на MSVC 14.39; полную сборку это не подтверждает.
## Статические операторы преобразования масок

В `ResourceBitset`, `SafetyMask128`, `ArchLegalityMask`, `BackendHazardMask` удалены ошибочные статические операторы преобразования в собственный тип вместе с их определениями. Их действие обеспечивают существующие конструкторы из соответствующих аргументов; конструкторы двух последних типов сделаны `explicit`, как исходные явные преобразования C#. У определения `ResourceBitset::operator unsigned long long` убран недопустимый вне класса `explicit`, оставленный в объявлении. Скрипт `fix-mask-conversions.ps1` выполняет проверку ожидаемого текста и повторно запускать его безопасно.

Проверка: MSVC 14.39 `/std:c++20 /Zs` на `mask-header-check.cpp`, включающем настоящие заголовки. До исправления было четыре C2801, после — ноль; полная проверка заголовков пока завершается ошибкой из-за `IEquatable` и других незавершённых частей. Результаты в `mask-before.log` и `mask-after.log`.
## Статические константы собственного типа

В `ResourceBitset`, `SafetyMask128`, `ArchLegalityMask` и `BackendHazardMask` объявления `Zero` и `All` оставлены в классе, а их `inline`-определения перенесены в конец заголовков после объявления соответствующих классов. Инициализаторы сохранены. Конструкторы `ResourceBitset`, определённые в `.cpp`, больше не помечены `inline`, чтобы были доступны другим единицам трансляции. Изменение и повторную проверку исходного текста выполняет `fix-mask-static-members.ps1`.

MSVC 14.39 `/std:c++20 /Zs` проверил реальные заголовки через `mask-constant-check.cpp`: до изменения диагностировал 12 ошибок C2027, после — код 0. В тесте временно объявлен совместимый интерфейс `IEquatable`, поскольку его конвертация в проекте ещё не завершена; эта изолированная проверка не подтверждает сборку или компоновку всего проекта. Журналы: `mask-constant-before.log` и `mask-constant-after.log`.
## Статические политики CompilerTypedSlotPolicy

Три значения `CompilerTypedSlotPolicy`, определённые внутри незавершённого класса, перенесены в `inline`-определения после объявления классов. Инициализаторы и доступ к закрытому конструктору сохранены. Повторно запускаемый скрипт: `fix-compiler-policy-static-members.ps1`.

MSVC 14.39 `/std:c++20 /Zs` на `compiler-contract-check.cpp`: до исправления шесть диагностик C2027; после исправления заголовок компилируется с кодом 0. Результаты — `contract-before.log` и `contract-after.log`; отдельная проверка исходного `CompilerContract.cpp` сохранена в `contract-cpp-after.log`. Это не проверка проекта целиком.
## Окончания строк заголовков Tangible

В шести заголовках `tangible_*.h` одиночный CR заменён на CRLF без изменения остальных байтов. До этого MSVC воспринимал определения после комментария как часть той же строки, например `StringHelper` не определялся. Скрипт `fix-tangible-line-endings.ps1` обрабатывает только одиночный CR и повторный запуск ничего не меняет. Проверка `string-helper-check.cpp` с MSVC 14.39: до C2653/C3861, после код 0 (`helper-before.log`, `helper-after.log`). Компилятор также предупреждает о существующих преобразованиях wchar_t в unsigned char в helper.

## Реализация CompilerContract.cpp

Сверено с оригиналом `HybridCPU_ISE/CloseToHSL/Core/Contracts/CompilerContract.cs`: поле Mode инициализируется в списке инициализации; шаблоны C# `is A or B` переведены в два сравнения; строка сообщения приведена к wide-формату и подключён заголовок InvalidOperationException. Скрипт `fix-compiler-contract.ps1`. Синтаксическая компиляция исходного файла MSVC 14.39 проходит (`contract-cpp-fixed.log`); `compiler-contract-test.cpp` собран и запущен вместе с исходником, проверены три политики и сценарии совпадающей и ошибочной версии (`contract-runtime.log`, код 0). Полная сборка проекта ещё не проверена.
## C# pattern `opcode is A or B`

Конвертер сформировал выражения `opcode == A or B`, которые пытаются объединить bool и значение enum. Для 33 выражений с простым локальным `opcode` преобразовано в `(opcode == A || opcode == B ...)`. Скобки сохраняют группировку при соседних `&&` и `||`. Скрипт `fix-opcode-pattern-or.ps1`: без параметров поиск, `-Apply` применяет изменения; строки с комментариями или строковыми литералами намеренно пропускаются. Повторный поиск: 0 найденных выражений по этому шаблону. Остальные формы с вызовами функций и другими переменными требуют отдельной обработки.

Для проверки `generate-opcode-pattern-check.ps1` извлекает два реальных выражения из `MatrixTileResourceContour.cpp` в минимальное окружение; MSVC 14.39 до исправления выдавал C2677, после — компиляция и запуск проверок выбранных и других кодов завершились кодом 0. Логи `opcode-pattern-before.log`, `opcode-pattern-after.log`. Это проверка выражений, не компиляция всех 33 файлов.
## DataTypeUtils

Сверено с `HybridCPU_ISE/Arch/DataType.cs`: заменён неперенесённый `Enum::IsDefined` на перечисление всех 14 допустимых значений; некорректное форматирование исключения заменено выводом числового значения enum с использованием существующего `ArgumentOutOfRangeException`. Удалён `inline` у функций, определения которых находятся в `.cpp`, чтобы их можно было вызывать из других единиц трансляции. Скрипт `fix-datatype.ps1`.

MSVC 14.39: исходный `DataType.cpp` до исправления не проходил компиляцию (`datatype-before.log`). После исправления он собран и скомпонован отдельно с `datatype-test.cpp`, тест выполняет проверки всех 256 значений базового байта, размеров, классификации и исключений; код возврата 0 (`datatype-after.log`). Это не подтверждает сборку всего проекта.
## Другие локальные переменные в C# `is A or B`

В 55 выражениях для простых идентификаторов (например, `reason`, `slotClass`, `kind`) исправлено `x == A or B` на `(x == A || x == B)`. Скрипт `fix-variable-pattern-or.ps1` по умолчанию только ищет, с `-Apply` выполняет замену. Он не обрабатывает строки с комментариями, строковыми литералами или оставшимся синтаксисом `is`/`and_Keyword`; сложные выражения требуют отдельной работы. Повторный поиск по охваченному шаблону: 0.

`generate-variable-pattern-check.ps1` извлекает две реальные строки из `SlotClassDefinitions.cpp`: до изменения компилятор MSVC 14.39 сообщал C2677, после созданный тест собран и выполнен с кодом 0, включая проверки отсутствующих в списке значений. Логи `variable-pattern-before.log`, `variable-pattern-after.log`. Остальные изменённые файлы этим тестом не компилировались.
## Отрицательный pattern C# `x is not (A or B)`

Исправлено 26 выражений с простой переменной и числовыми или квалифицированными константами на `(x != A && x != B)`. `fix-negative-pattern.ps1` без параметров проверяет исходники, а `-Apply` вносит изменения; повторный поиск по охваченному шаблону даёт ноль. Более сложные выражения и строки с комментариями или литералами не обрабатываются. Тест `generate-negative-pattern-check.ps1` извлекает условие из `MemoryCycleController.cpp`; до изменения MSVC сообщал ошибки синтаксиса, после исправления компиляция и выполнение теста завершились кодом 0. Логи `negative-pattern-before.log`, `negative-pattern-after.log`.

## Проверка CpuSecondStageNeutralAuxiliary::TryDecode

Сверено с исходным C# `CpuSecondStageTranslationMechanism.cs`: восстановлены диапазон kind, допустимые accessKind и проверка уровня pageWalkLevel с нужной группировкой. Скрипт `fix-second-stage-decode.ps1`. `generate-second-stage-check.ps1` извлекает реальные реализации Encode/TryDecode и компилирует их с минимальными объявлениями типов; тест проверяет все комбинации 8 видов, 5 доступов, 3 размеров и 4 уровней и отклонение зарезервированного бита. MSVC 14.39, код 0 (`second-stage-check.log`). Весь исходный файл и проект этим тестом не собирались.
## Диапазоны C# `is >= A and < B`

В трёх местах исправлены диапазоны `register_Keyword`, `issueLane.getPhysicalLaneIndex()` и `candidate->VirtualThreadId`. Для `issueLane` результат вызова сохраняется, чтобы вычисление происходило ровно один раз. Границы сверены с исходными C#. Скрипт `fix-range-patterns.ps1`; `generate-range-check.ps1` извлекает первые два условия и проверяет граничные значения и однократный вызов метода. Сборка и запуск MSVC 14.39 — код 0 (`range-check.log`); целые исходные файлы этим тестом не собирались. Выражение `seed::Register` не изменено, поскольку окружающий LINQ также не преобразован.

## PowerControlCsr::DecodePowerState

Восстановлено тело switch из оригинального `Arch/PowerControlCsr.cs`: пять допустимых C-состояний и `ErrorState` для остальных значений без усечения входного 64-битного числа. Скрипт `fix-power-decode.ps1`; `generate-power-decode-check.ps1` извлекает реальную реализацию и компилирует с минимальными определениями. Проверены пять значений, соседние недопустимые и значение выше 32 бит, код 0 (`power-decode-check.log`). Заголовок реального процессора ещё содержит независимые ошибки конвертации, поэтому весь файл данным тестом не собран.
## Базовый тип CorePowerState

Тип `CorePowerState` исправлен с `int` на `unsigned int` в соответствии с `CPU_Core.FSM.cs`, где исходный тип — `uint`. Это сохраняет значение `ErrorState = 0xFFFFFFFF`. Скрипт `fix-power-enum.ps1`; `generate-power-enum-check.ps1` извлекает реальное перечисление. До исправления MSVC выдавал C4369, после исправления компиляция с `/WX /Zs` проходит (`power-enum-before.log`, `power-enum-after.log`).

## C# `getter() is A or B`

Исправлено 20 выражений с вызовом метода без аргументов и перечислением квалифицированных констант: результат метода сохраняется в немедленно вызываемой лямбде и сравнивается с каждым значением. Это сохраняет однократный вызов и краткое замыкание окружающего условия. Скрипт `fix-getter-pattern-or.ps1`: по умолчанию поиск, `-Apply` замена; повторный поиск — 0 по этому шаблону. Строки с комментариями и литералами пропускаются.

`generate-getter-pattern-check.ps1` извлекает реальное условие из `SecureCompletionPublicationFence.cpp`. До изменения MSVC сообщал C2677, после собранный тест проверяет все три значения и однократность вызова getter (код 0; логи `getter-pattern-before.log`, `getter-pattern-after.log`). Остальные изменённые единицы этим тестом не компилировались.
## InstructionClassifier: восстановление fallback-классификации

В `InstructionClassifier.cpp` восстановлены два непереведённых выражения switch из сохранённых конвертером строк C#: 150 правил `InstructionClass`, 80 правил `SerializationClass` и оба варианта по умолчанию. Логика предварительного поиска в GeneratedIsaCatalog сохранена. У `GetClass` и `GetSerializationClass` удалён `inline` в объявлениях и расположенных в `.cpp` определениях для нормальной компоновки. Скрипт `fix-instruction-classifier.ps1` проверяет число распознанных вариантов и сохраняет исходные ожидаемые пары в `classifier-expected.csv`.

`generate-classifier-check.ps1` извлекает фактические условия из исправленного файла и создаёт тест с минимальными объявлениями перечислений: 230 правил и два значения по умолчанию проверены, MSVC 14.39 сборка и запуск — код 0 (`classifier-check.log`). Проверка фактического `InstructionClassifier.cpp` по-прежнему не проходит из-за ошибок подключённых заголовков, включая отсутствующий `IEquatable` (`classifier-before.log`, `classifier-after.log`); тест не проверяет интеграцию с GeneratedIsaCatalog и реальные численные значения opcode.
## IEquatable и сравнение полей диагностики

В существующих исходниках C++ `IEquatable<T>` использовался только в наследовании 18 классов и ограничении шаблона `DifferentialTraceCompare::CheckField`; хранение ссылок/указателей на интерфейс не найдено. Поэтому наследование несуществующего .NET-интерфейса и только связанные с ним спецификаторы `override` удалены; сами методы `Equals` сохранены. Это не добавляет виртуальных таблиц к аппаратно ориентированным структурам. Скрипт `convert-iequatable-classes.ps1` проверяет соответствие класса и типа аргумента Equals.

Ограничение CheckField заменено на C++20: интегральные значения сравниваются через `==` и форматируются как числа (bool как True/False), остальные значения должны предоставлять Equals и ToString. Скрипт `convert-checkfield-constraint.ps1`. Такая адаптация покрывает найденное использование в текущем проекте, но не предоставляет динамическое приведение к C#-интерфейсу; при добавлении такого использования потребуется отдельная реализация.

MSVC 14.39 компилирует реальные заголовки масок без заглушек и подтверждает `sizeof(ResourceBitset)==sizeof(SafetyMask128)==16` (`iequatable-mask-after.log`). `generate-iequatable-checkfield-test.ps1` извлекает сам шаблон CheckField; компиляция и тест bool, int, byte и пользовательского типа прошли (`iequatable-checkfield.log`). После изменения первая ошибка компиляции реального InstructionClassifier сместилась на отсутствующее пространство имён Registers и ReadOnlySpan (`iequatable-classifier-after.log`); полная компиляция этого исходника и проекта ещё не проходит.
## SlotClassLaneMap и ReadOnlySpan

По исходному `SlotClassDefinitions.cs` восстановлены маски линий, вычисление ёмкости, связи алиасов и их проверка. `ReadOnlySpan<SlotClass>` заменён на `std::span<const SlotClass>`; представления ссылаются на статические неизменяемые массивы, а не на временные объекты. Методы перенесены в самостоятельный `SlotClassLaneMap.cpp`, включённый в vcxproj; вызов в `SlotClassCapacity.cpp` использует `auto`. `fix-slot-class-lane-map.ps1` выполняет настройки проекта и замену существующих объявлений; файл реализации хранится в дереве исходников.

Реальный новый файл собран MSVC 14.39 и скомпонован с `slot-class-lane-map-test.cpp`; проверены семь классов, маски, ёмкость, симметричные алиасы, время жизни представления и исключение для неизвестного класса (`slot-class-lane-map-test.log`, код 0). Intel icx-cl 2025.0 проверил синтаксис этого файла (`slot-class-intel-check.log`, код 0; есть предупреждение в стороннем exception helper). До исправления проверка реального заголовка выдавала C7568 для ReadOnlySpan (`slot-class-header-before.log`). Остальные 824 употребления Span/ReadOnlySpan автоматически не менялись: у них есть дополнительные методы и ограничения времени жизни.
## Выделение SlotClassCapacityState и зависимости MicroOp

`SlotClassCapacity.h` использует MicroOp только через `shared_ptr`, поэтому включение полного `MicroOp.h` перенесено в исходник ComputeFromBundle; предварительное объявление уже доступно через SlotClassDefinitions.h. Методы независимого состояния вынесены без изменения логики в `SlotClassCapacityState.cpp`, добавленный в vcxproj. У двух методов, определяемых в `.cpp`, убран `inline`, чтобы обеспечить компоновку. Скрипт `split-slot-capacity-state.ps1`.

Реальные `SlotClassCapacityState.cpp` и `SlotClassLaneMap.cpp` собраны и скомпонованы MSVC 14.39 с тестом `slot-capacity-state-test.cpp`. Проверены ёмкости, занятость, взаимное влияние алиасов, сброс и сохранение суммарной ёмкости (`slot-capacity-state-test.log`, код 0). Intel icx-cl 2025.0 синтаксически проверил файл состояния (`slot-capacity-intel.log`, код 0; предупреждение о необработанном Unclassified соответствует существующему отсутствию действия). До выделения полная единица `SlotClassCapacity.cpp` не компилировалась из-за каскада ошибок в MicroOp (`slot-capacity-before.log`); её метод ComputeFromBundle по-прежнему зависит от ещё не исправленного MicroOp и полной сборкой не проверен.
## VLIW_Instruction, VLIW_Bundle и InvalidOpcodeException

`ReadOnlySpan`/`Span` в этих двух классах переведены в `std::span<const unsigned char>`/`std::span<unsigned char>`; чтение/запись слов восстановлено в little-endian, с проверкой размера и отрицательного offset. Восстановлены локальные функции PackField/TryDecodeField (архитектурные регистры 0–31 и маркер NoArchReg), удалены неиспользуемые using и исправлено форматирование диагностики. У Bundle использованы `subspan` и соответствующие проверки размера. Скрипты `fix-vliw-instruction.ps1`, `fix-vliw-bundle.ps1`.

Исправлены конструкторы `InvalidOpcodeException`: теперь инициализируются `std::runtime_error` и константные поля. Сообщение преобразуется в UTF-8; внутреннее исключение сохраняется в `exception_ptr` и доступно через `getInnerException`. Входной параметр внутреннего исключения остаётся унаследованным от конвертера значением `std::runtime_error`, поэтому производный тип в этом конструкторе может быть срезан. Скрипт `fix-invalid-opcode-exception.ps1`.

Реальные три `.cpp` собраны и скомпонованы MSVC 14.39 с `vliw-roundtrip-test.cpp`: проверены little-endian roundtrip, короткие буферы, отрицательный offset, запрещённые биты, упаковка регистров, пакет из восьми инструкций и внутреннее исключение (`vliw-roundtrip-test.log`, код 0). Intel icx-cl 2025.0 выполнил синтаксическую проверку всех трёх файлов (`vliw-intel-check.log`, код 0, предупреждения о codecvt). До исправления реальная единица VLIW не компилировалась (`vliw-instruction-before.log`); полная сборка проекта остаётся незавершённой.
## Реализации ResourceBitset и SafetyMask128

Преобразования C# `obj is T` исправлены через `std::any_cast` с проверкой типа; форматирование масок приведено к wide-строкам и 16 шестнадцатеричным разрядам. `HashCode.Combine` заменён детерминированным C++-хешем, согласованным с равенством; его числовые значения не претендуют на совпадение с .NET HashCode.Combine. У определённых в `.cpp` методов ResourceBitset удалён `inline`, чтобы код можно было компоновать из других единиц трансляции. Скрипт `fix-mask-implementations.ps1`.

До исправления две реальные единицы не проходили MSVC (`masks-cpp-before.log`). После исправления они собраны и скомпонованы с `masks-implementation-test.cpp`: проверены сравнение через std::any, согласованность хеша, строки, конфликты чтение/запись и чтение/чтение, банки и побитовые операции (`masks-implementation-test.log`, код 0). Intel icx-cl 2025.0 проверил синтаксис обоих файлов (`masks-implementation-intel.log`, код 0). Проект целиком ещё не собран.
## CanonicalDecodedContracts: ненужные директивы using

Удалены четыре директивы using несуществующих на момент включения пространств имён: в активных объявлениях заголовка они не используются. Скрипт `fix-canonical-unused-usings.ps1`. Повторная синтаксическая проверка InstructionClassifier MSVC перешла к ошибкам ImmutableArray (`canonical-usings-after.log`); реализация CanonicalDecodedContracts остаётся незавершённой, в частности C# record, JSON, SHA и коллекции.

## ArchLegalityMask и SafetyMask128

Добавлена константность методам SafetyMask128, которые не изменяют объект; в ArchLegalityMask и BackendHazardMask исправлены any_cast и форматирование. Скрипт `fix-arch-mask-const.ps1`. Реальные ArchLegalityMask.cpp и SafetyMask128.cpp собраны и скомпонованы с тестом сравнений, конфликтов и объединения (`arch-mask-test.log`, код 0). Для простого теста std::wstring с Intel 2025.0 и -fintelfpga ошибка wchar_t не воспроизвелась (`wchar-before.log`); для установления её причины необходима полная диагностическая команда сборки.
## CanonicalDecodeContext

Из не переведённого C# record восстановлено объявление полей CanonicalDecodeContext в отдельном заголовке; тип ошибки DecodeFailureCode изменён на unsigned short в соответствии с исходным ushort. Объявление подключено к CanonicalDecodedContracts.h; определение Unbound добавлено в существующий CPP и берёт версию и хеш непосредственно из GeneratedIsaCatalog. Скрипт `fix-canonical-context.ps1`. В отличие от C# init-only полей, поля C++ пока доступны для записи, и это необходимо учесть при последующей конвертации модели владения.

`canonical-context-test.cpp` собирается и выполняется на MSVC (поля, копирование и нулевые значения), Intel icx-cl проверяет синтаксис (`canonical-context-test.log`, `canonical-context-intel.log`). Интеграция Unbound и полная компиляция CanonicalDecodedContracts.cpp пока не подтверждены: остаются непереведённые ImmutableArray/JSON и ошибки генерированного каталога.
## SHA-256 для CanonicalPayloadSnapshot

Добавлен Sha256Digest.cpp/h на Windows BCrypt с возвращением 64 строчных шестнадцатеричных символов. Метод CanonicalPayloadSnapshot::Hash переведён на std::span<const unsigned char> и вызывает его; новый исходник и bcrypt.lib подключены к проекту. Скрипт `fix-canonical-sha256.ps1` настраивает подключения. Для SHA API использованы очистка ресурсов и обработка ошибок, данные обрабатываются блоками, допустимыми BCrypt.

Реальная реализация Sha256Digest.cpp собрана MSVC и проверена на стандартных векторах пустой строки и abc (`sha256-test.log`, код 0); Intel icx-cl проверил синтаксис (`sha256-intel.log`). Весь CanonicalDecodedContracts.cpp всё ещё не компилируется из-за неготовых ImmutableArray, JSON и записей, поэтому интеграционная компоновка метода Hash ещё не проверена.
## CanonicalPayloadSnapshot: операции над байтами

В отдельные CanonicalPayloadSnapshot.h/cpp перенесены владение байтами, создание из std::span, SHA-256, равенство и хеш; файл добавлен в проект скриптом `fix-canonical-payload.ps1`. Вход копируется, getContent возвращает владеющую копию, чтобы данные не зависели от времени жизни snapshot. Это отличается по стоимости от C# ReadOnlyMemory; оставшийся вызов ShadowSemanticProjection и другие потребители требуют адаптации. Числовое значение хеша отличается от .NET, но согласовано с равенством. Непереведённые FromObject и Deserialize остаются незавершёнными и не заглушены.

Реальные CanonicalPayloadSnapshot.cpp и Sha256Digest.cpp собраны MSVC и протестированы на копирование, неизменяемость, SHA, равенство и пустой kind (`canonical-payload-test.log`, код 0). Intel icx-cl проверил синтаксис snapshot (`canonical-payload-intel.log`). Вся CanonicalDecodedContracts.cpp и проект остаются не собраны из-за оставшегося C# JSON и других конструкций.
## SemanticInstructionKey

По исходному C# выделен SemanticInstructionKey.h/cpp: копируются байты и значения контекста, проверяются непустые факторы, сравниваются все факторы и реальные байты; SHA вычисляется ранее добавленным Sha256Hex. Скрипт `fix-semantic-key.ps1` подключает исходник и устраняет старые непереведённые определения. `getRawBundleBytes` возвращает копию вместо C# ReadOnlyMemory; потребители должны быть адаптированы. GetHashCode согласован с равенством, но его значения отличаются от .NET и допускают дополнительные коллизии.

MSVC собрал реальные SemanticInstructionKey.cpp и Sha256Digest.cpp и выполнил `semantic-key-test.cpp` (копирование, изменение исходных данных/контекста, равенство, хеш, проверка пустого фактора), код 0. Intel icx-cl проверил синтаксис (`semantic-key-intel.log`). Полная сборка декодера не проверена: другие record, JSON и вызовы коллекций ещё не переведены.
## FPGA-компиляция wchar_t в Tangible StringHelper

Полная команда MSBuild показала, что ошибки `is_unsigned_v<wchar_t>`, `_Mark`, `_Match` возникают из `find_first_not_of` и `find_last_not_of` при компиляции Intel FPGA device code. Скрипт `fix-intel-wchar-trim.ps1` заменяет два вызова на посимвольный поиск, сохраняя обработку пустой строки, строки только из удаляемых символов и пользовательского набора trimChars.

Intel icx-cl 2025.0 с `/fsycl /fintelfpga /c`: `wchar-helper-device.cpp` до исправления воспроизводил все три ошибки, после компилируется (`wchar-helper-before.log`, `wchar-helper-after.log`). MSVC-тест `wchar-trim-test.cpp` прошёл, включая граничные случаи. Повторная реальная компиляция CanonicalDecodedContracts через MSBuild больше не содержит этих трёх ошибок (`current-msbuild-after-trim.log`), но остаются другие ошибки декодера, поэтому сборка завершается с кодом 1.
## DecodeFailure

Восстановлена пропущенная при конвертации C# record DecodeFailure с неизменяемыми полями и Create: проверяются непустые field/message, хешируются исходные байты через Sha256Hex. DecodeFailureCode сохранён как unsigned short. `fix-decode-failure.ps1` подключает тип и исходник к проекту.

Реальные DecodeFailure.cpp и Sha256Digest.cpp собраны и проверены MSVC тестом полей, SHA-256 и отказа для пустого field (`decode-failure-test.log`, код 0). Intel icx-cl 2025.0 проверен с `/fsycl /fintelfpga /c` (`decode-failure-intel.log`). Конвертация и сборка вызывающих файлов декодера пока не завершена.
## Rf06ExecutionContracts: цикл заголовков

В Rf06ExecutionContracts.h добавлено предварительное объявление CanonicalScalarLoadAddressPlan до включения CanonicalDecodedContracts.h: внутри RF06 тип используется через shared_ptr. Удалены три директивы using, не используемые активными объявлениями заголовка и ссылающиеся на ещё не объявленные namespace в цепочке включений. Скрипт `fix-rf06-canonical-forward.ps1`.

Повторная компиляция CanonicalDecodedContracts.cpp средствами MSBuild/Intel 2025.0 больше не сообщает ошибки необъявленного CanonicalScalarLoadAddressPlan и этих трёх using (`rf06-forward-after.log`, `rf06-usings-after.log`). Сам исходник всё ещё не собирается из-за других непереведённых типов и ImmutableArray, поэтому это не подтверждает сборку проекта.
## LaneId и ExecutionPlacement

Восстановлен пропущенный C# record LaneId с проверкой диапазона 0–7, TryCreate, преобразованием и форматированием (скрипт `fix-lane-id.ps1`). Восстановлен ExecutionPlacement с фабриками Create/CreateHardPinned и TryGetHardPinnedLaneId, проверкой закреплённой линии и обнулением линии для гибкого размещения; подключение — `fix-execution-placement.ps1`. В C++ конструктор выбрасывает std::out_of_range вместо .NET ArgumentOutOfRangeException.

`lane-id-test.cpp` проверил размеры и диапазон -1..8 на MSVC; `execution-placement-test.cpp` проверил оба вида размещения, область, некорректную линию и извлечение LaneId (код 0). Intel icx-cl с `/fsycl /fintelfpga /c` также скомпилировал тест (`execution-placement-intel.log`). Сборка вызывающих RF06 исходников и всего проекта остаётся незавершённой.
## MemoryCapability

Восстановлены пропущенные MemoryBankId, FrozenMemoryRange, MemoryAccessDirection и MemoryCapability, включая проверки банка, переполнения диапазона, направления и пересечений, сортировку footprint и NoMemory. Заголовок подключён скриптом `fix-memory-capability.ps1`. Представление использует const vector вместо ImmutableArray и std::optional вместо nullable; вызывающие файлы ещё требуют адаптации к C++ API. Исключения C++ отличаются по типу от .NET.

`memory-capability-test.cpp` собран и запущен MSVC: проверены сортировка, банковые границы, переполнение, пересечения и отсутствие обязательных параметров. Intel icx-cl с `/fsycl /fintelfpga /c` успешно скомпилировал тест (`memory-capability-test.log`, `memory-capability-intel.log`). Полная интеграционная сборка RF06 не подтверждена.
## Идентификаторы привязок RF06

Восстановлены MaterializerId, RuntimeExecutionProviderId, LatencyModelId и RuntimeExecutionProviderBinding с проверкой и обрезкой строк; в C++ поля объявлены const. Скрипт `fix-execution-binding-identity.ps1` подключает заголовок. GeneratedStaticBinding пока только предварительно объявлен: его фабрика и вычисление отпечатка зависят от незавершённого GeneratedIsaCatalog и JSON.

MSVC собрал и выполнил тест идентификаторов и обязательной схемы, Intel icx-cl с `/fsycl /fintelfpga /c` скомпилировал тест (`binding-identity-test.log`, `binding-identity-intel.log`). Повторная компиляция CanonicalDecodedContracts.cpp средствами MSBuild устранила ошибки отсутствующих имён привязок в объявлениях и дошла до ImmutableArray (`rf06-binding-after.log`); полная сборка ещё не проходит.
## RF06: коллекции регистров

В ExecutionContract заменены ImmutableArray<int> на копируемые std::vector<int> с const-хранением, возвращением копии и проверкой отрицательных идентификаторов. Конструктор теперь инициализирует const-поля через список инициализации; параметры создания принимаются по значению, чтобы поддерживать временные и пустые векторы. Скрипт `fix-rf06-register-collections.ps1`. Сам RF06 CPP пока содержит другие непереведённые конструкции; преобразование вызывающих мест продолжается.

Тест реального `FreezeRegisterIds` проверил копирование, пустую коллекцию и отклонение отрицательного значения на MSVC; Intel icx-cl с `/fsycl /fintelfpga /c` скомпилировал тест (`freeze-registers-test.log`, `freeze-registers-intel.log`). В выбранной компиляции CanonicalDecodedContracts сообщения об ImmutableArray<int> в ExecutionContract исчезли, следующие ошибки относятся к отсутствующим AdmissionProjectionFingerprint и LegacyAdmissionProjection (`rf06-registers-after.log`). Полная сборка не прошла.
## Типы LegacyAdmissionProjection и AdmissionProjectionFingerprint

Восстановлены отсутствующие типы проекции и отпечатка: проекция хранит копии списков и неизменяемые поля, отпечаток проверяет и обрезает строку. Скрипт `fix-legacy-projection-types.ps1`. Placement представлен shared_ptr<const ExecutionPlacement>, поэтому незавершённые места использования требуют адаптации; LegacyProjectionGuard::Capture/EnsureCurrent пока не переведены, и безопасность проверки устаревшей проекции не подтверждена.

MSVC собрал и выполнил тест копирования и проверки отпечатка (`legacy-projection-test.log`, код 0). Повторная выбранная компиляция CanonicalDecodedContracts.cpp через Intel/MSBuild записана в `legacy-projection-after.log`; проект пока не компилируется.
## LegacyProjectionGuard

Восстановлены Capture и EnsureCurrent из C#: сериализация полей проекции в строку с именами enum, числами, списками и True/False, SHA-256 и сравнение значений отпечатка. Вместо ошибочного сравнения shared_ptr сравниваются значения. `fix-legacy-projection-guard.ps1` подключает отдельный CPP к проекту. Неизвестные значения enum форматируются числом, как .NET; проверка отпечатка выполняется только на неизменяемом снимке LegacyAdmissionProjection.

MSVC собрал реальные LegacyProjectionGuard.cpp, Sha256Digest.cpp и ResourceBitset.cpp с тестом эталонного SHA, совпадающей и изменённой проекции (`legacy-guard-test.log`, код 0). Intel icx-cl с `/fsycl /fintelfpga /c` проверил исходник (`legacy-guard-intel.log`). Интеграция вызывающих RF06 файлов и полная сборка пока не подтверждены.
## Счётчик OperationAttemptIssuer

Перенесён в отдельный заголовок и восстановлена проверка переполнения, соответствующая C# checked(++counter); прежний C++ вариант молча переполнялся. Для VliwOperationId и AdmissionRecord добавлены предварительные объявления, необходимые для shared_ptr, их реализация остаётся незавершённой. Скрипт `fix-attempt-issuer.ps1`.

MSVC собрал и выполнил проверку первых номеров двух независимых счётчиков (`attempt-issuer-test.log`). Достижение максимума в этом тесте не выполнялось. Повторная выборочная сборка Intel/MSBuild больше не останавливается на объявлениях этих указателей и переходит к ошибкам заголовков MicroOp (`attempt-issuer-msbuild.log`); полная сборка не проходит.
## Исключения MicroOp

В MicroOp.Exceptions удалены неиспользуемые директивы using, восстановлены конструкторы std::runtime_error и инициализация const-полей, сообщения и доступ к сохранённому внутреннему исключению. Для UTF-8 используется std::wstring_convert; API входного innerException по-прежнему принимает std::runtime_error по значению, поэтому производный тип может срезаться. Скрипт `fix-microop-exceptions.ps1`.

Реальный CPP собран и скомпонован MSVC с тестом полей, сообщения и вложенного исключения; Intel icx-cl с `/fsycl /fintelfpga /c` скомпилировал исходник (`microop-exceptions-test.log`, `microop-exceptions-intel.log`). Остальные MicroOp и полный проект этим тестом не собраны.
## Зависимости MemoryUnit.h

Тяжёлое циклическое включение Processor.h перенесено в MemoryUnit.cpp; для используемого через shared_ptr InstructionIR добавлено предварительное объявление в исходном пространстве имён. Неиспользуемый using удалён. Конструктор по умолчанию ResolvedMemoryAccess теперь явно задаёт нулевые значения полей, как default C# struct. Скрипт `fix-memory-unit-header.ps1`.

Intel icx-cl 2025.0 собрал `memory-unit-header-test.cpp` с `/fsycl /fintelfpga /c`, включая static_assert доступности default-конструктора (`memory-unit-header-test.log`). Выборочная сборка CanonicalDecodedContracts.cpp больше не выводит прежние первые ошибки MemoryUnit.h и переходит к Rf07ExecutionOutcomeContracts и не переведённым типам (`memory-unit-msbuild-after.log`). Реализация MemoryUnit.cpp и весь проект пока не собирались успешно.