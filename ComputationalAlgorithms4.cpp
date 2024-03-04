#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "exprtk.hpp"

int GetAmountOfValues(std::string FileName);
std::vector<double> GetArgumentValuesArray(std::ifstream& File, int AmountOfValues);
std::vector<std::vector<double>> GetFunctionValueTable(std::ifstream& File, int AmountOfValues);
void TableOutput(std::vector<std::vector<double>> Table, std::ostream& Stream);
std::vector<std::vector<double>> GetTableByFunction(std::vector<double> Arguments, std::string expression_str);
std::vector<exprtk::expression<double>> MakeExpressionTable(std::vector<exprtk::symbol_table<double>> SymbolTable,
    std::string ExpressionString, int Size);
void VectorOutput(std::vector<double> Vector, std::ostream& Stream);
std::vector<double> GetDeviations(std::vector<std::vector<double>> Table);
std::vector<std::vector<double>> GetSplineCoefficients(std::vector<std::vector<double>> Table);
std::vector<double> GetIntervals(std::vector<double> X);
std::vector<double> SweepMethod(std::vector<std::vector<double>> Matrix, std::vector<double> FreeMembersColumn);
double GetFunctionBySpline(std::vector<std::vector<double>> Table, std::vector<std::vector<double>> SplineCoefficients, double PointX);
std::vector<std::vector<double>> GetMatrixForComputingC(std::vector<std::vector<double>> Table);
std::vector<double> SetAlphas(std::vector<std::vector<double>> Matrix, std::vector<double> FreeMembersColumn);
std::vector<double> SetBettas(std::vector<std::vector<double>> Matrix, std::vector<double> FreeMembersColumn, 
                              std::vector<double> Alphas);

//splineoutput and GetFunctionBySpline наверно неправильные


int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    //Получение количества значений
    std::string AmountOfValuesFileName = "AmountOfValues.txt";
    int AmountOfValues = GetAmountOfValues(AmountOfValuesFileName);
    //Получение таблицы значений (для режима 1)
    std::string FunctionValueTableFileName = "FunctionValueTable.txt";
    std::ifstream FunctionValueTableFile;
    FunctionValueTableFile.open(FunctionValueTableFileName);
    //Получение массива аргументов (для режима 2)
    std::string ArgumentValuesArrayFileName = "ArgumentValuesArray.txt";
    std::ifstream ArgumentValuesArrayFile;
    ArgumentValuesArrayFile.open(ArgumentValuesArrayFileName);

    std::cout << "Выберите режим работы программы: " << std::endl;
    std::cout << "1) По заданной таблице значений функции определять приближенное значение функции в некоторой точке." << std::endl;
    std::cout << "2) По заданной аналитически функции и массиву значений аргумента вычислить таблицу значений функции." << std::endl;
    char SelectedMode;
    std::cout << "Режим: ";
    std::cin >> SelectedMode;

    std::string Function; //Строка для ввода функции (для режима 2)
    std::vector<std::vector<double>> Table(2, std::vector<double>(AmountOfValues)); //Таблица значений, используюется в двух режимах
    std::vector<double> ArgumentValuesArray(AmountOfValues); //Массив значений аргументов (для режима 2)
    std::vector<double> Deviations(AmountOfValues); //Отклонения
    double MaxDeviation = 0.0; //Максимальное отклонение
    double j = 0.1;
    switch (SelectedMode)
    {
    case '1':
        Table = GetFunctionValueTable(FunctionValueTableFile, AmountOfValues); //Заполнение таблицы из файла
        std::cout << "Таблица значений функции: " << std::endl;
        TableOutput(Table, std::cout);
        std::cout << "Введите точку для определения приближенного значения функции: ";
        double PointX;
        std::cin >> PointX;
        for (int i = 0; i < 40; i++) {
            std::cout << PointX << std::endl;
            PointX += 0.05;
        }
        PointX = -1.0;
        std::cout << std::endl;
        for (int i = 0; i < 40; i++) {
            std::cout << GetFunctionBySpline(Table, GetSplineCoefficients(Table), PointX) << std::endl;
            PointX += 0.05;
        }
        break;
    case '2':
        std::cout << "Пример функции: sqrt(abs(x))" << std::endl;
        std::cout << "Введите функцию в аналитическом виде: y=";
        std::cin >> Function;
        ArgumentValuesArray = GetArgumentValuesArray(ArgumentValuesArrayFile, AmountOfValues); //Получение массива аргументов
        Table = GetTableByFunction(ArgumentValuesArray, Function);
        std::cout << "Таблица значений функции: " << std::endl;
        TableOutput(Table, std::cout);
        Deviations = GetDeviations(Table);
        std::cout << std::endl << "Отклонения: " << std::endl;
        VectorOutput(Deviations, std::cout);
        for (int i = 0; i < Deviations.size(); i++)
            MaxDeviation = std::max(MaxDeviation, Deviations[i]);
        std::cout << "Максимальное отклонение: " << std::endl;
        std::cout << MaxDeviation;
        break;
    default:
        std::cout << "Режим не был выбран." << std::endl;
    }
    FunctionValueTableFile.close();
    ArgumentValuesArrayFile.close();
    return 0;
}
//Получить количество аргументов из файла
int GetAmountOfValues(std::string FileName)
{
    std::ifstream AmountOfValuesFile;
    int AmountOfValues = 0;
    try
    {
        AmountOfValuesFile.open(FileName);
        AmountOfValuesFile >> AmountOfValues;
        AmountOfValuesFile.close();
    }
    catch (const std::exception& Exception)
    {
        std::cout << Exception.what() << std::endl;
    }
    return AmountOfValues;
}
//Ввод таблицы значений аргументов и функции
std::vector<std::vector<double>> GetFunctionValueTable(std::ifstream& File, int AmountOfValues)
{
    std::vector<std::vector<double>> Table(2, std::vector<double>(AmountOfValues));
    for (int i = 0; i < 2; i++)
        Table[i] = GetArgumentValuesArray(File, AmountOfValues);
    return Table;
}
//Ввод массива значений аргумента
std::vector<double> GetArgumentValuesArray(std::ifstream& File, int AmountOfValues)
{
    std::vector<double> Arguments(AmountOfValues);
    try
    {
        for (int i = 0; i < AmountOfValues; i++)
            File >> Arguments[i];
    }
    catch (const std::exception& Exception)
    {
        std::cout << Exception.what() << std::endl;
    }
    return Arguments;
}
//Вывод таблицы значений функции и аргумента
void TableOutput(std::vector<std::vector<double>> Table, std::ostream& Stream)
{
    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
            Stream << "x\t\t";
        else
            Stream << "y\t\t";
        for (int j = 0; j < Table[i].size(); j++)
        {
            Stream << Table[i][j] << "\t\t";
        }
        Stream << std::endl << std::endl;
    }
}
//Получает таблицу по введенной в аналитическом виде функции
std::vector<std::vector<double>> GetTableByFunction(std::vector<double> Arguments, std::string ExpressionString)
{
    std::vector<exprtk::symbol_table<double>> SymbolTable(Arguments.size());
    for (int i = 0; i < Arguments.size(); i++) { //Заполнение таблицы символов
        SymbolTable[i].add_variable("x", Arguments[i]);
        SymbolTable[i].add_function("abs", abs);
        SymbolTable[i].add_function("sqrt", sqrt);
        SymbolTable[i].add_function("exp", exp);
    }
    //Создаются выражения, вычисляются значения функции
    std::vector<exprtk::expression<double>> Expression = MakeExpressionTable(SymbolTable, ExpressionString, Arguments.size());
    std::vector<std::vector<double>> Table(2, std::vector<double>(Arguments.size()));
    // Заполняет таблицу аргументами и значениями функции
    for (int i = 0; i < Arguments.size(); i++)
    {
        Table[0][i] = Arguments[i];
        Table[1][i] = Expression[i].value();
    }
    return Table;
}
//Создает выражения, вычисляются значения функции
std::vector<exprtk::expression<double>> MakeExpressionTable(std::vector<exprtk::symbol_table<double>> SymbolTable,
    std::string ExpressionString, int Size)
{
    //Создается вектор выражений заданного размера
    std::vector<exprtk::expression<double>> Expression(Size);
    for (int i = 0; i < Size; i++)
        Expression[i].register_symbol_table(SymbolTable[i]);
    //Компилируется каждое выражение с помощью парсера
    exprtk::parser<double> Parser;
    for (int i = 0; i < Size; i++)
        if (!Parser.compile(ExpressionString, Expression[i]))
            std::cout << "Ошибка в выражении!\n";
    return Expression; //Возвращается вектор скомпилированных выражений
}
//Подсчитывает отклонения
std::vector<double> GetDeviations(std::vector<std::vector<double>> Table)
{
    std::vector<double> Deviations(Table[0].size());
    for (int i = 0; i < Table[0].size(); i++) {
        Deviations[i] = fabs(Table[1][i] - GetFunctionBySpline(Table, GetSplineCoefficients(Table), Table[0][i]));
        std::cout << "y = " << Table[1][i];
        std::cout << " = " << GetFunctionBySpline(Table, GetSplineCoefficients(Table), Table[0][i]) << std::endl;
    }
    return Deviations;
}
//Вывод вектора
void VectorOutput(std::vector<double> Vector, std::ostream& Stream)
{
    for (int i = 0; i < Vector.size(); i++)
        Stream << Vector[i] << std::endl;
}
//Метод прогонки
std::vector<double> SweepMethod(std::vector<std::vector<double>> Matrix, std::vector<double> FreeMembersColumn)
{
    std::vector<double> Alphas = SetAlphas(Matrix, FreeMembersColumn);
    std::vector<double> Bettas = SetBettas(Matrix, FreeMembersColumn, Alphas);
    int N = Matrix[0].size()-1;
    std::vector<double> X(Matrix[0].size());
    for (int i = Matrix[0].size() - 1; i >= 0; i--)
    {
        if (i == Matrix[0].size() - 1) {
            X[i] = Bettas[i];
        }
        else {
            X[i] = Alphas[i] * X[i + 1] + Bettas[i];
        }
    }
    //костыль
    for (int i = 0; i < X.size(); i++)
        X[i] /= 2.0;
    return X;
}
//Устанавливает значения альфа
std::vector<double> SetAlphas(std::vector<std::vector<double>> Matrix, std::vector<double> FreeMembersColumn)
{
    std::vector<double> Alphas(FreeMembersColumn.size());
    //Проходим до Matrix[0].size() - 1 т.к. последнее С = 0 -> A = 0
    for (int i = 0; i < Matrix[0].size() - 1; i++)
    {
        if (i == 0) 
            Alphas[i] = -Matrix[i][i + 1] / Matrix[i][i]; // C[0] / B[0]
        else 
            Alphas[i] = -Matrix[i][i + 1] / (Matrix[i][i] + Matrix[i][i - 1] * Alphas[i - 1]);
    }
    return Alphas;
}
//Устанавливает значения бетта
std::vector<double> SetBettas(std::vector<std::vector<double>> Matrix, std::vector<double> FreeMembersColumn,
                              std::vector<double> Alphas)
{
    std::vector<double> Bettas(Matrix[0].size());
    //Проходим до Matrix[0].size() - 1 т.к. последнее С = 0 -> A = 0
    for (int i = 0; i < Matrix[0].size() - 1; i++)
    {
        if (i == 0) 
            Bettas[i] = FreeMembersColumn[i] / Matrix[i][i]; // -D[0] / B[0]
        else
            Bettas[i] = (FreeMembersColumn[i] - Matrix[i][i - 1] * Bettas[i - 1]) / (Matrix[i][i] + Matrix[i][i - 1] * Alphas[i - 1]);
    }
    int N = Matrix[0].size() - 1;
    Bettas[N] = (FreeMembersColumn[N] - Matrix[N][N - 1] * Bettas[N - 1]) / (Matrix[N][N] + Matrix[N][N - 1] * Alphas[N - 1]);
    return Bettas;
}
//Вычисляет интервалы между значениями X
std::vector<double> GetIntervals(std::vector<double> X)
{
    std::vector<double> H(X.size() - 1);
    for (int i = 0; i < H.size(); ++i) {
        H[i] = X[i+1] - X[i];
    }
    return H;
}
//Вычисляет коэффициенты B, C, D
std::vector<std::vector<double>> GetSplineCoefficients(std::vector<std::vector<double>> Table)
{
    std::vector<double> H = GetIntervals(Table[0]);
    std::vector<double> FreeMembersColumn(Table[1].size());
    for (int i = 1; i < FreeMembersColumn.size()-2; ++i) //Из СЛАУ для нахождения С
        FreeMembersColumn[i] = 6 * ((Table[1][i + 1] - Table[1][i]) / H[i+1] - (Table[1][i] - Table[1][i - 1]) / H[i]);
    std::vector<double> C = SweepMethod(GetMatrixForComputingC(Table), FreeMembersColumn); //Нахождение С методом прогонки
    std::vector<double> D(Table[1].size()-1);  //-1 потому что количество интервалов меньше на 1 чем переменных
    std::vector<double> B(Table[1].size() - 1); //-1 потому что количество интервалов меньше на 1 чем переменных
    for (int i = 0; i < D.size(); i++) {
        D[i] = (C[i + 1] - C[i]) / (3.0*H[i]);
        B[i] = (Table[1][i+1] - Table[1][i])/H[i] - (H[i]/3.0)*(2.0*C[i]+C[i+1]); //???
    }
    return {Table[1], B, C, D};
}
//Вычисляет значение y используя сплайн
double GetFunctionBySpline(std::vector<std::vector<double>> Table, std::vector<std::vector<double>> SplineCoefficients, double PointX)
{
    if (PointX == Table[0][0])
        return SplineCoefficients[0][0];
    /*for (int i = 1; i < Table[0].size(); ++i) {
        if (Table[0][i - 1] <= PointX && PointX <= Table[0][i]) {
            /*std::cout << "S=" << SplineCoefficients[0][i] << "+" << SplineCoefficients[1][i - 1] << "*(" <<
                PointX << "-" << Table[0][i] << ")+" << SplineCoefficients[2][i - 1] << "/2*("
                << PointX << "-" << Table[0][i] << ")^2+" << SplineCoefficients[3][i - 1] << "/6*(" <<
                PointX << "-" << Table[0][i] << ")^3" << std::endl;
            return SplineCoefficients[0][i] + SplineCoefficients[1][i - 1] * (PointX - Table[0][i]) + SplineCoefficients[2][i - 1] / 2.0
                * (PointX - Table[0][i]) * (PointX - Table[0][i]) + SplineCoefficients[3][i - 1] / 6.0 * (PointX - Table[0][i]) * 
                (PointX - Table[0][i]) * (PointX - Table[0][i]);
        }
    } */

    //По формуле верно, но невзки появляются 
    
    for (int i = 0; i < Table[0].size()-1; ++i) {
        if (Table[0][i] <= PointX && PointX <= Table[0][i+1]) {
            return SplineCoefficients[0][i] + SplineCoefficients[1][i] * (PointX - Table[0][i]) + SplineCoefficients[2][i] / 2
                * (PointX - Table[0][i]) * (PointX - Table[0][i]) + SplineCoefficients[3][i] / 6 * (PointX - Table[0][i]) *
                (PointX - Table[0][i]) * (PointX - Table[0][i]);
        }
    }
    return 0.0;
}
//Создает матрицу для вычисления коэффициентов С
std::vector<std::vector<double>> GetMatrixForComputingC(std::vector<std::vector<double>> Table)
{
    std::vector<double> H = GetIntervals(Table[0]);
    std::vector<std::vector<double>> Matrix(Table[0].size(), std::vector<double>(Table[0].size()));
    for (int i = 0; i < Table[0].size(); ++i) {
        std::vector<double> Row(Table[0].size(), 0.0);
        if (i == 0 || i == Table[0].size() - 1) {
            Row[i] = 1.0;
        }
        else {
            Row[i - 1] = H[i - 1];
            Row[i] = 2 * (H[i] + H[i - 1]);
            Row[i + 1] = H[i];
        }
        Matrix[i] = Row;

    }
    return Matrix;
}