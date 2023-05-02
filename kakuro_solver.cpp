#include <iostream>
#include <string>

#include <fstream>
#include <sstream>
#include <vector>

#include "stdc++.h"
#include <array>
#include "/usr/local/opt/libomp/include/omp.h"

using namespace std;

#define NUM_THREADS 4

enum direction
{
    d_down,
    d_right,
    none
};

#define COORD std::pair<int, int>

// #define DEBUG

int iter = 0;

/// Auxiliary functions

void display_arr(int *arr, int n)
{

    cout << "arr: ";

    for (int i = 0; i < n; i++)
    {
        cout << arr[i] << " ";
    }

    cout << endl;
}

void print_coords(COORD start, COORD end)
{

    cout << "Start:" << start.first << "," << start.second << endl;
    cout << "End:" << end.first << "," << end.second << endl;
}

int find_length(COORD start, COORD end, direction dir)
{

    if (dir == d_down)
        return end.first - start.first;
    if (dir == d_right)
        return end.second - start.second;

    return -1;
}

void convert_sol(int **mat, int **&sol_mat, int m, int n)
{

    sol_mat = new int *[m]; // Rows
    for (int i = 0; i < m; i++)
    {
        sol_mat[i] = new int[n]; // Cols
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (mat[i][j] == -2)
                sol_mat[i][j] = -2; // Empty value cell
            else
                sol_mat[i][j] = -1; // Hint or empty cell
        }
    }
}

void print_one_matrix(int **matrix, int m, int n)
{
    std::cout << "Matrix: " << std::endl;
    for (int i = 0; i < m; i++)
    { // rows
        for (int j = 0; j < n; j++)
        { // cols
            std::cout << matrix[i][j] << "\t";
        }
        std::cout << "\n";
    }
}

void sol_to_file(int **mat, int **sol_mat, int m, int n, string fname)
{

    // string fname = "visualize.kakuro";
    ofstream to_write(fname);

    to_write << m << " " << n << "\n";

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (mat[i][j] != -2)
                to_write << mat[i][j] << " ";
            else
                to_write << sol_mat[i][j] << " ";
        }
        to_write << "\n";
    }

    to_write.close();
}

void read_matrix(int **&matrix, std::ifstream &afile, int m, int n)
{

    matrix = new int *[m]; // rows

    for (int i = 0; i < m; i++)
    {
        matrix[i] = new int[n]; // cols
    }

    int val;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            afile >> val;
            matrix[i][j] = val;
        }
    }
}

/// Auxiliary functions

struct sum
{
    COORD start;
    COORD end;

    int hint;
    int dir;
    int length;
    int *arr;

    void print_sum()
    {
        cout << "############################" << endl;
        cout << "Creating sum with: " << endl;
        print_coords(start, end);
        cout << "Hint: " << hint << endl;
        cout << "Direction: " << dir << endl;
        cout << "Length: " << length << endl;
        cout << "############################" << endl;
    }

    sum(COORD _start, COORD _end, int _hint, direction _dir) : start(_start), end(_end), hint(_hint), dir(_dir)
    {
        length = find_length(_start, _end, _dir);
        arr = new int[length];
#ifdef DEBUG
        cout << "############################" << endl;
        cout << "Creating sum with: " << endl;
        print_coords(start, end);
        cout << "Hint: " << hint << endl;
        cout << "Direction: " << dir << endl;
        cout << "Length: " << length << endl;
        cout << "############################" << endl;
#endif
    }

    //~sum(){
    // delete arr;
    //}
};

COORD find_end(int **matrix, int m, int n, int i, int j, direction dir)
{ // 0 down 1 right

    if (dir == d_right)
    {
        for (int jj = j + 1; jj < n; jj++)
        {
            if (matrix[i][jj] != -2 || jj == n - 1)
            {
                if (matrix[i][jj] == -2 && jj == n - 1)
                    jj++;
                COORD END = COORD(i, jj);
                return END;
            }
        }
    }

    if (dir == d_down)
    {
        for (int ii = i + 1; ii < m; ii++)
        {
            if (matrix[ii][j] != -2 || ii == m - 1)
            {
                if (matrix[ii][j] == -2 && ii == m - 1)
                    ii++;
                COORD END = COORD(ii, j);
                return END;
            }
        }
    }
}

vector<sum> get_sums(int **matrix, int m, int n)
{

    vector<sum> sums;

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int val = matrix[i][j];
            if (val != -1 && val != -2)
            {
                int hint = val;
                hint = hint / 10;

                if ((hint % 100) == 0)
                {
                    hint = (int)(hint / 100);
                    COORD START = COORD(i, j + 1);
                    COORD END = find_end(matrix, m, n, i, j, d_right);
                    sum _sum = sum(START, END, hint, d_right);
                    sums.push_back(_sum);
                }

                else
                {
                    int div = (int)(hint / 100);
                    int rem = (int)(hint % 100);

                    if (div == 0 && rem != 0)
                    {
                        COORD START = COORD(i + 1, j);
                        COORD END = find_end(matrix, m, n, i, j, d_down);
                        sum _sum = sum(START, END, rem, d_down);
                        sums.push_back(_sum);
                    }

                    if (div != 0 && rem != 0)
                    {
                        COORD START1 = COORD(i + 1, j);
                        COORD START2 = COORD(i, j + 1);
                        COORD END1 = find_end(matrix, m, n, i, j, d_down);
                        COORD END2 = find_end(matrix, m, n, i, j, d_right);
                        sum _sum1 = sum(START1, END1, rem, d_down);
                        sum _sum2 = sum(START2, END2, div, d_right);
                        sums.push_back(_sum1);
                        sums.push_back(_sum2);
                    }
                }
            }
        }
    }
    return sums;
}

// Enum for sums.
// Success: it is valid or has potential to be valid.
// Over: Sum of values in sum cells are so big that filling remaining cells cannot produce a valid result.
// Under: Sum of values in sum cells are so small that filling remaining cells cannot produce a valid result.
enum sumStatus
{
    success,
    over,
    under
};

sumStatus checkSumStatus(int remaining_sum, int remaining_cells)
{
    int current_max_num = 9;
    int current_min_num = 1;
    int max_num = 0;
    int min_num = 0;

    for (int i = 0; i < remaining_cells; i++)
    {
        max_num += current_max_num;
        min_num += current_min_num;
        current_max_num--;
        current_min_num++;
    }

    // remaining_sum > maximum value that can fit into remaining_cells:.
    // We need to put bigger values to cells: anything containing smaller nums will be wrong
    if (remaining_sum > max_num)
        return sumStatus::under;

    // remaining_sum < minimum value that can fit into remaining_cells:.
    // We need to put smaller values to cells: anything containing bigger nums will be wrong
    if (remaining_sum < min_num)
        return sumStatus::over;

    return sumStatus::success;
}

// Checks the solution matrix whether it is valid or has potential to be valid for a given sum object.
sumStatus checkSum(int **sol_mat, sum _sum)
{
    int hint = _sum.hint;
    // COORD.first is the row index
    // COOORD.second is the column index
    int row_idx = _sum.start.first;
    int col_idx = _sum.start.second;

    // Check for a row sum
    if (_sum.dir == direction::d_right)
    {
        int end_idx = _sum.end.second;

        // Continue iteration until there is a currently empty cell or end of the sum region.
        while (col_idx < end_idx && sol_mat[row_idx][col_idx] > 0)
        {
            // Substract the remaining sum by the value inside the sum region.
            hint -= sol_mat[row_idx][col_idx];
            sumStatus status = checkSumStatus(hint, end_idx - col_idx - 1);
            // If sum status is not valid, return the status.
            if (status != sumStatus::success)
                return status;
            col_idx++;
        }
    }

    // Check for a column sum
    else
    {
        int end_idx = _sum.end.first;

        // Continue iteration until there is a currently empty cell or end of the sum region.
        while (row_idx < end_idx && sol_mat[row_idx][col_idx] > 0)
        {
            // Substract the remaining sum by the value inside the sum region.
            hint -= sol_mat[row_idx][col_idx];
            sumStatus status = checkSumStatus(hint, end_idx - row_idx - 1);
            // If sum status is not valid, return the status.
            if (status != sumStatus::success)
                return status;
            row_idx++;
        }
    }

    return sumStatus::success;
}

// Checks a given row whether it has duplicates.
bool checkRow(int **sol_mat, int row_index, int m, int n)
{
    vector<bool> checks(9, true);
    for (int i = 0; i < 9; i++)
    {
        checks[i] = false;
    }

    for (int i = 0; i < n; i++)
    {
        if (sol_mat[row_index][i] > 0)
        {
            if (checks[sol_mat[row_index][i] - 1])
                return false;
            checks[sol_mat[row_index][i] - 1] = true;
        }
    }
    return true;
}

// 3D array to map board cells to the sums they are included in.
vector<vector<vector<sum *>>> setCell2Sums(vector<sum> &sums, int m, int n)
{
    vector<vector<vector<sum *>>> cell_2_sums(m, vector<vector<sum *>>(n, vector<sum *>()));

    for (int i = 0; i < sums.size(); i++)
    {
        int start_row = sums[i].start.first;
        int start_col = sums[i].start.second;
        int end_row = sums[i].end.first;
        int end_col = sums[i].end.second;

        sum *tmp = &(sums[i]);
        if (sums[i].dir == direction::d_right)
        {
            for (int j = start_col; j < end_col; j++)
            {
                cell_2_sums[start_row][j].push_back(tmp);
            }
        }
        else
        {
            for (int j = start_row; j < end_row; j++)
            {
                cell_2_sums[j][start_col].push_back(tmp);
            }
        }
    }

    return cell_2_sums;
}

bool solution(int **mat, int **sol_mat, vector<sum> sums, int m, int n)
{
    // TO DO: Write the solution
    // You can use any algorithm and data type
    // Write your solution to file in main function using sol_to_mat() after solving it
#pragma omp parallel num_threads(NUM_THREADS)
    {
        cout << omp_get_thread_num();
#pragma omp task
        {
        }
    }

    return false;
}

int main(int argc, char **argv)
{

    std::string filename(argv[1]);
    std::ifstream file;
    file.open(filename.c_str());

    int m, n;
    file >> m;
    file >> n;

    int **mat;
    read_matrix(mat, file, m, n);
    print_one_matrix(mat, m, n);

    int **sol_mat;
    convert_sol(mat, sol_mat, m, n);
    print_one_matrix(sol_mat, m, n);

    vector<sum> sums = get_sums(mat, m, n);
    solution(mat, sol_mat, sums, m, n);
    print_one_matrix(sol_mat, m, n);
    sol_to_file(mat, sol_mat, m, n, "solution.kakuro");

    for (int i = 0; i < n; i++)
    {
        delete mat[i];
        delete sol_mat[i];
    }

    delete mat;
    delete sol_mat;

    return 0;
}
