#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <sstream>
#include <string>
#include <algorithm>

constexpr size_t MAX_PAGES = 100;

using RulesBitmap = std::vector<std::bitset<MAX_PAGES>>;
using Line = std::vector<int>;

void parse_rules(std::ifstream &file, RulesBitmap &rules_bitmap)
{
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            break;
        std::istringstream iss(line);
        int before, after;
        char delimiter;
        iss >> before >> delimiter >> after;
        rules_bitmap[before].set(after);
    }
}

void parse_updates(std::ifstream &file, std::vector<Line> &updates)
{
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;
        std::istringstream iss(line);
        Line update;
        std::string num;
        while (std::getline(iss, num, ','))
        {
            update.push_back(std::stoi(num));
        }
        updates.push_back(update);
    }
}

bool validate_line(const Line &line, const RulesBitmap &rules_bitmap)
{
    for (size_t i = 0; i < line.size() - 1; ++i)
    {
        if (!rules_bitmap[line[i]].test(line[i + 1]))
        {
            return false;
        }
    }
    return true;
}

struct PageComparator
{
    const RulesBitmap &rules_bitmap;

    PageComparator(const RulesBitmap &bitmap) : rules_bitmap(bitmap) {}

    bool operator()(int a, int b) const
    {
        if (rules_bitmap[a].test(b))
        {
            return false;
        }
        if (rules_bitmap[b].test(a))
        {
            return true;
        }
        return a > b;
    }
};

void process_updates(
    const std::vector<Line> &updates,
    const RulesBitmap &rules_bitmap,
    int &valid_sum,
    int &invalid_sum)
{
    valid_sum = 0;
    invalid_sum = 0;

    for (const auto &line : updates)
    {
        if (validate_line(line, rules_bitmap))
        {
            valid_sum += line[line.size() / 2];
        }
        else
        {
            Line sorted_line = line;
            std::sort(sorted_line.begin(), sorted_line.end(), PageComparator(rules_bitmap));
            invalid_sum += sorted_line[sorted_line.size() / 2];
        }
    }
}

int main()
{
    std::ifstream file("input.txt");
    if (!file.is_open())
    {
        std::cerr << "Failed to open the input file." << std::endl;
        return 1;
    }

    RulesBitmap rules_bitmap(MAX_PAGES);
    parse_rules(file, rules_bitmap);

    std::vector<Line> updates;
    parse_updates(file, updates);

    int valid_sum = 0;
    int invalid_sum = 0;
    process_updates(updates, rules_bitmap, valid_sum, invalid_sum);

    std::cout << "Valid Sum: " << valid_sum << std::endl;
    std::cout << "Invalid Sum: " << invalid_sum << std::endl;

    return 0;
}
