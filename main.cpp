#include <windows.h>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <ctime>

using std::cout;
using std::endl;
using std::string;
using std::vector;

enum class opType { add, edit, remove, retrieve };

int getRandomIntInRange(int from, int to) {
    return (from + std::rand() % (to - from + 1)); // NOLINT(cert-msc50-cpp)
}

// Выводит отчет по записям
template<typename F, typename S>
void displayEntries(const std::map<F, S> &source) {
    cout << "Отчет:" << endl;
    for (const auto &[key, value] : source) cout << key << ": " << value << endl;
}

// В зависимости от операции выводит на дисплей результат
void printLog(opType mode, bool status, const string &keyword, int itemsAmount = 0) {
    vector<string> operationNames = { "Добавление", "Изменение", "Удаление", "Извлечение" };
    string operation = operationNames[static_cast<int>(mode)];
    const char* msg = "LOG:                  %s по ключу %s %s";
    const char* onSuccess = "прошло успешно.\n";
    string onSuccessExtended = "прошло успешно. Количество обработанных записей: " + std::to_string(itemsAmount) + "\n";
    vector<string> causes = { "такая запись уже существует", "такой записи не существует"};
    string onFailure = "не удалось, т.к. " + ((mode == opType::add) ? causes[0] : causes[1]) + "\n";

    if (status) {
        if (itemsAmount == 0) printf(msg, operation.c_str(), keyword.c_str(), onSuccess);
        else printf(msg, operation.c_str(), keyword.c_str(), onSuccessExtended.c_str());
    }
    else printf(msg, operation.c_str(), keyword.c_str(), onFailure.c_str());
}

// Добавить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool addEntryToMap(const std::pair<F, S> &entry, std::map<F, S> &target) {
    if (target.count(entry.first) == 1) return false;

    target.insert(entry);

    return true;
}

// Изменить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool changeEntryInMap(const std::pair<F, S> &entry, std::map<F, S> &target) {
    if (target.count(entry.first) == 0) return false;

    auto it = target.find(entry.first);
    it->second = entry.second;

    return true;
}

// Вернуть одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool retrieveMapValueByKey(S &target, const F &key, const std::map<F, S> &source) {
    const auto it = source.find(key);

    if (it == source.end()) return false;

    target = it->second;

    return true;
}

// Удалить одну запись. Возвращает false, если записи по указанному ключу не обнаружено
template<typename F, typename S>
bool removeEntryFromMap(const F &key, std::map<F, S> &target) {
    const auto it = target.find(key);

    if (it == target.end()) return false;

    target.erase(it);

    return true;
}

// Функции Extended аналогичны базовым прототипам, но имеют ещё логирование
template<typename F, typename S>
bool addEntryToMapExtend(const std::pair<F, S> &entry, std::map<F, S> &target) {
    auto status = addEntryToMap(entry, target);
    printLog(opType::add, status, entry.first);
    return status;
}

template<typename F, typename S>
bool changeEntryInMapExtended(const std::pair<F, S> &entry, std::map<F, S> &target) {
    auto status = changeEntryInMap(entry, target);
    printLog(opType::edit, status, entry.first);
    return status;
}

template<typename F, typename S>
bool retrieveMapValueByKeyExtended(S &target, const F &key, const std::map<F, S> &source) {
    auto status = retrieveMapValueByKey(target, key, source);
    printLog(opType::retrieve, status, key);
    return status;
}

template<typename F, typename S>
bool removeEntryFromMapExtended(const F &key, std::map<F, S> &target) {
    auto status = removeEntryFromMap(key, target);
    printLog(opType::remove, status, key);
    return status;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::srand(std::time(nullptr)); // NOLINT(cert-msc51-cpp)

    vector<string> list = {
            "Ahmedov", "Bubnov", "Filippov", "Ivanov", "Izmailov", "Krupitsin", "Kuritsin", "Mazurov",
            "Nosov", "Opalenko", "Ramadanov", "Rollov", "Victorenko", "Zyrianov", "Yarov",
            "NEXT", "NEXT"
            };

    std::map<string, int> queue;

    for (int i = 0; i < 20; ++i) {
        int index = getRandomIntInRange(0, (int)list.size() - 1);

        if (list[index] == "NEXT") {
            cout << "Команда: " << index << endl;
            if (list.empty()) continue;

            int currentAmount;
            string key = queue.begin()->first;

            // Получить value первый элемент. Это будет текущее количество
            retrieveMapValueByKeyExtended(currentAmount, key, queue);
            // Если больше 1, тогда уменьшить
            if (currentAmount > 1) {
                changeEntryInMapExtended(std::make_pair(key, currentAmount - 1), queue);
            }
            else {
                // Если равно 1, тогда удалить
                removeEntryFromMapExtended(key, queue);
            }
            continue;
        }

        cout << "В очередь встал " << index << ": " << list[index] << endl;
        // Часть с добавлением
        std::pair<string, int> entry = { list[index], 1 };

        bool status = addEntryToMapExtend(entry, queue);
        // Если добавить не удалось, значит запись с таким ключом уже существует...
        if (!status) {
            int currentAmount;
            // Получаем текущее количество в этом ключе...
            retrieveMapValueByKeyExtended(currentAmount, entry.first, queue);
            // Увеличиваем на один...
            entry.second = currentAmount + 1;
            // И меняем текущую запись
            changeEntryInMapExtended(entry, queue);
        }
    }

    displayEntries(queue);
}
