#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <map>



class LEVEL2{
public:
	std::vector<std::string> date;
    std::vector<std::string> timestamp;
    std::vector<std::string> order_num;
    std::vector<std::string> event;
    std::vector<std::string> ticker;
    std::vector<double> price;
    std::vector<int> qty;
    std::vector<std::string> mpid;
    std::vector<std::string> exchange;

	void historical_data_parse(std::string data_file){
	    std::ifstream csv_data;
	    csv_data.open(data_file);
	    if(!csv_data.is_open()){
	        std::cout<<"Error! Unable to open historical data!"<<std::endl;
	        exit(1);
	    }
	    else{
	        std::cout<<"Historical data opened!"<<std::endl;
	    }
	    bool initial = true;
	    std::string line;
    	std::string current;
	    while(getline(csv_data, line)){
	        if(initial){
	            initial = false;
	            continue;
	        }
	        std::stringstream ss(line);
	        getline(ss, current, ',');
	        date.push_back(current);
	        getline(ss, current, ',');
	        timestamp.push_back(current);
	        getline(ss, current, ',');
	        order_num.push_back(current);
	        getline(ss, current, ',');
	        event.push_back(current);
	        getline(ss, current, ',');
	        ticker.push_back(current);
	        getline(ss, current, ',');
	        price.push_back(std::stod (current));
	        getline(ss, current, ',');
	        qty.push_back(std::stoi (current));
	        getline(ss, current, ',');
	        mpid.push_back(current);
	        getline(ss, current, ',');
	        exchange.push_back(current);
    	}
	}
};


struct BID_ASK {
    std::vector<double> bid_price;
    std::vector<int> bid_qty;
    std::vector<std::string> bid_exchange;
    std::vector<std::string> bid_id;
    std::vector<double> ask_price;
    std::vector<int> ask_qty;
    std::vector<std::string> ask_exchange;
    std::vector<std::string> ask_id;
};
typedef struct BID_ASK BID_ASK;


class DATA{
public:
	std::map<std::string, BID_ASK> bid_ask;

	template <class T>
	static void reorder(std::vector<T>& vA, std::vector<size_t>& vI){
	    // Quoted from StackOverflow user rcgldr, under Creative Commons
	    size_t i, j, k;
	        for(i = 0; i < vA.size(); i++){
	            while(i != (j = vI[i])){
	                k = vI[j];
	                std::swap(vA[j], vA[k]);
	                std::swap(vI[i], vI[j]);
	        }}
	}

	template <typename T>
	static std::vector<size_t> sort_indexes(const std::vector<T> &v) {
	    // Quoted from StackOverflow user Lukasz Wiklendt, under Creative Commons
	    // initialize original index locations
	    std::vector<size_t> idx(v.size());
	    std::iota(idx.begin(), idx.end(), 0);
	    // sort indexes based on comparing values in v
	    sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
	    return idx;
	}

	static void sort_bid_ask(BID_ASK *ba){
	    std::vector<long unsigned int> index;
	    std::vector<long unsigned int> index_copy;
	    if(ba->bid_price.size() > 0){
	        index = sort_indexes(ba->bid_price);
	        std::reverse(index.begin(), index.end());
	        index_copy = index;
	        reorder(ba->bid_price, index_copy);
	        index_copy = index;
	        reorder(ba->bid_qty, index_copy);
	        index_copy = index;
	        reorder(ba->bid_exchange, index_copy);
	        index_copy = index;
	        reorder(ba->bid_id, index_copy);
	    }
	    if(ba->ask_price.size() > 0){
	        index = sort_indexes(ba->ask_price);
	        index_copy = index;
	        reorder(ba->ask_price, index_copy);
	        index_copy = index;
	        reorder(ba->ask_qty, index_copy);
	        index_copy = index;
	        reorder(ba->ask_exchange, index_copy);
	        index_copy = index;
        	reorder(ba->ask_id, index_copy);
    	}
	}

	static void print_bid_ask(BID_ASK &data){
	    std::cout<<"BID\tSIZE\tEXCHANGE\tASK\tSIZE\tEXCHANGE"<<std::endl;
	    int len_bid = data.bid_price.size();
	    int len_ask = data.ask_price.size();
	    int max_len;
	    if(len_bid > len_ask){max_len = len_bid;}
	    else{max_len = len_ask;}
	    for(int i = 0; i < max_len; i++){
	        if(i < len_bid){std::cout<< data.bid_price[i] << "\t" << data.bid_qty[i] << "\t" << data.bid_exchange[i] << "\t\t";}
	        else{for(int q = 0; q < 6; q++){std::cout<<"\t";}}
	        if(i < len_ask){std::cout<< data.ask_price[i] << "\t" << data.ask_qty[i] << "\t" << data.ask_exchange[i] <<"\n";}
	        else{for(int q = 0; q < 5; q++){std::cout<<"\t";}; std::cout<<"\n";}
	    }
	    std::cout<<"\n";
	}

	void generate_bid_ask(LEVEL2 &data){
	    BID_ASK ba;
	    std::string timestamp;
	    double price;
	    int qty;
	    std::string event;
	    std::string exchange;
	    std::string id;
	    int i = 0;
	    while(i < data.price.size()){
	        timestamp = data.timestamp[i];
	        while(data.timestamp[i] == timestamp){
	            price = data.price[i];
	            qty = data.qty[i];
	            event = data.event[i];
	            exchange = data.exchange[i];
	            id = data.order_num[i];
	            if(data.event[i] == "ADD BID"){
	                ba.bid_price.push_back(price);
	                ba.bid_qty.push_back(qty);
	                ba.bid_exchange.push_back(exchange);
	                ba.bid_id.push_back(id);
	            }
	            else if(data.event[i] == "ADD ASK"){
	                ba.ask_price.push_back(price);
	                ba.ask_qty.push_back(qty);
	                ba.ask_exchange.push_back(exchange);
	                ba.ask_id.push_back(id);
	            }
	            else if(data.event[i] == "DELETE BID" || data.event[i] == "FILL BID"){
	                auto it = std::find(ba.bid_id.begin(), ba.bid_id.end(), id);
	                int index = std::distance(ba.bid_id.begin(), it);
	                ba.bid_price.erase(ba.bid_price.begin() + index);
	                ba.bid_qty.erase(ba.bid_qty.begin() + index);
	                ba.bid_exchange.erase(ba.bid_exchange.begin() + index);
	                ba.bid_id.erase(ba.bid_id.begin() + index);
	            }
	            else if(data.event[i] == "DELETE ASK" || data.event[i] == "FILL ASK"){
	                auto it = std::find(ba.ask_id.begin(), ba.ask_id.end(), id);
	                int index = std::distance(ba.ask_id.begin(), it);
	                ba.ask_price.erase(ba.ask_price.begin() + index);
	                ba.ask_qty.erase(ba.ask_qty.begin() + index);
	                ba.ask_exchange.erase(ba.ask_exchange.begin() + index);
	                ba.ask_id.erase(ba.ask_id.begin() + index);
	            }
	            else if(data.event[i] == "CANCEL BID" || data.event[i] == "EXECUTE BID"){
	                auto it = std::find(ba.bid_id.begin(), ba.bid_id.end(), id);
	                int index = std::distance(ba.bid_id.begin(), it);
	                ba.bid_qty[index] -= qty;
	            }
	            else if(data.event[i] == "CANCEL ASK" || data.event[i] == "EXECUTE ASK"){
	                auto it = std::find(ba.ask_id.begin(), ba.ask_id.end(), id);
	                int index = std::distance(ba.ask_id.begin(), it);
	                ba.ask_qty[index] -= qty;
	            }
	            else if(data.event[i] == "TRADE BID"){
	                // executing non-displayed order
	            }
	            else if(data.event[i] == "TRADE ASK"){
	                // executing non-displayed order
	            }
	            ++i;
	            sort_bid_ask(&ba);
	        }
	    bid_ask.insert(std::make_pair(timestamp, ba));
	    }
   	}
};


int main(){
	LEVEL2 l2;
	l2.historical_data_parse("YOUR-DATA-HERE.csv");
	// sample data available from AlgoSeek website: https://www.algoseek.com/samples/

	DATA data;
	data.generate_bid_ask(l2);

	// print bid-ask with timestamp
	for(auto it=data.bid_ask.begin(); it!=data.bid_ask.end(); ++it){
		std::cout<<it->first<<std::endl;
		data.print_bid_ask(it->second);
	}
}
