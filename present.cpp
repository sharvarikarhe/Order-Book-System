#include "Orderbook.h"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <charconv>
#include <tuple>
#include <stdexcept>

// --- Input Parsing Logic (Self-Contained Version) ---

enum class ActionType { Add, Cancel, Modify };

struct Information 
{
    ActionType type_;
    OrderType orderType_;
    Side side_;
    Price price_;
    Quantity quantity_;
    OrderId orderId_;
};

using Informations = std::vector<Information>;

struct InputHandler
{
private:
    std::uint32_t ToNumber(const std::string_view& str) const
    {
        std::int64_t value{};
        std::from_chars(str.data(), str.data() + str.size(), value);
        if (value < 0)
            throw std::logic_error("Value is below zero.");
        return static_cast<std::uint32_t>(value);
    }

    std::vector<std::string_view> Split(const std::string_view& str, char delimeter) const
    {
        std::vector<std::string_view> columns;
        columns.reserve(5);
        std::size_t start_index{}, end_index{};
        while ((end_index = str.find(delimeter, start_index)) && end_index != std::string::npos)
        {
            auto distance = end_index - start_index;
            auto column = str.substr(start_index, distance);
            start_index = end_index + 1;
            columns.push_back(column);
        }
        columns.push_back(str.substr(start_index));
        return columns;
    }

    Side ParseSide(const std::string_view& str) const
    {
        if (str == "B") return Side::Buy;
        if (str == "S") return Side::Sell;
        throw std::logic_error("Unknown Side");
    }

    OrderType ParseOrderType(const std::string_view& str) const
    {
        if (str == "FillAndKill") return OrderType::FillAndKill;
        if (str == "GoodTillCancel") return OrderType::GoodTillCancel;
        if (str == "GoodForDay") return OrderType::GoodForDay;
        if (str == "FillOrKill") return OrderType::FillOrKill;
        if (str == "Market") return OrderType::Market;
        throw std::logic_error("Unknown OrderType");
    }

    Price ParsePrice(const std::string_view& str) const
    {
        if (str.empty()) throw std::logic_error("Unknown Price");
        return ToNumber(str);
    }

    Quantity ParseQuantity(const std::string_view& str) const
    {
        if (str.empty()) throw std::logic_error("Unknown Quantity");
        return ToNumber(str);
    }

    OrderId ParseOrderId(const std::string_view& str) const
    {
        if (str.empty()) throw std::logic_error("Empty OrderId");
        return static_cast<OrderId>(ToNumber(str));
    }

    bool TryParseInformation(const std::string_view& str, Information& action) const
    {
        auto value = str.at(0);
        auto values = Split(str, ' ');
        if (value == 'A')
        {
            action.type_ = ActionType::Add;
            action.side_ = ParseSide(values[1]);
            action.orderType_ = ParseOrderType(values[2]);
            action.price_ = ParsePrice(values[3]);
            action.quantity_ = ParseQuantity(values[4]);
            action.orderId_ = ParseOrderId(values[5]);
        }
        else if (value == 'M')
        {
            action.type_ = ActionType::Modify;
            action.orderId_ = ParseOrderId(values[1]);
            action.side_ = ParseSide(values[2]);
            action.price_ = ParsePrice(values[3]);
            action.quantity_ = ParseQuantity(values[4]);
        }
        else if (value == 'C')
        {
            action.type_ = ActionType::Cancel;
            action.orderId_ = ParseOrderId(values[1]);
        }
        else return false;
        return true;
    }

public:
    Informations GetActionsOnly(const std::filesystem::path& path) const
    {
        Informations actions;
        actions.reserve(1000);
        std::string line;
        std::ifstream file{ path };
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#' || line[0] == 'R')
                continue;

            Information action;
            if (TryParseInformation(line, action))
                actions.push_back(action);
        }
        return actions;
    }
};

// --- Main Program Logic ---

void PrintResults(const Orderbook& book)
{
    auto infos = book.GetOrderInfos();
    std::cout << "\n--- Final State of the Order Book ---\n";
    std::cout << "Total Orders Remaining: " << book.Size() << std::endl;

    std::cout << "\n--- Bids (Buy Orders) --- [" << infos.GetBids().size() << " levels]\n";
    for (const auto& level : infos.GetBids()) {
        std::cout << "Price: " << level.price_ << ",  Total Quantity: " << level.quantity_ << std::endl;
    }

    std::cout << "\n--- Asks (Sell Orders) --- [" << infos.GetAsks().size() << " levels]\n";
    for (const auto& level : infos.GetAsks()) {
        std::cout << "Price: " << level.price_ << ",  Total Quantity: " << level.quantity_ << std::endl;
    }
    std::cout << "-------------------------------------\n";
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./present <path_to_scenario_file>" << std::endl;
        return 1;
    }

    std::filesystem::path scenario_file = argv[1];
    if (!std::filesystem::exists(scenario_file))
    {
        std::cerr << "Error: File not found - " << scenario_file << std::endl;
        return 1;
    }

    std::cout << "Processing scenario file: " << scenario_file.filename().string() << "\n";

    InputHandler handler;
    Orderbook orderbook;

    try
    {
        const auto actions = handler.GetActionsOnly(scenario_file);

        for (const auto& action : actions)
        {
            switch (action.type_)
            {
                case ActionType::Add:
                    orderbook.AddOrder(std::make_shared<Order>(action.orderType_, action.orderId_, action.side_, action.price_, action.quantity_));
                    break;
                case ActionType::Cancel:
                    orderbook.CancelOrder(action.orderId_);
                    break;
                case ActionType::Modify:
                    orderbook.ModifyOrder(OrderModify{action.orderId_, action.side_, action.price_, action.quantity_});
                    break;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    PrintResults(orderbook);

    return 0;
}