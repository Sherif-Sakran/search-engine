#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;
int key = 0;
int rankP = 1;
////splitting the sentence into words
vector<string> split(string input);

struct website {
	string name;
	vector<int> pointTo;
	vector<int> pointedBy;
	int id;
	int clicks = 0;
	int impressions = 1;
	double CTR = 0;
	int pageRank = 0;
	double score = 0;
	vector<string> keywords;
};

int main() {
	ifstream myfile;
	myfile.open("sites.csv");
	if (!myfile.is_open())
	{
		cout << "Error while opening the file\n";
		return 0;
	}

	vector<website> sites;
	string n;
	string name1, name2;
	while (myfile.good())
	{
		getline(myfile, n);
		int index = n.find(',');
		name1 = n.substr(0, index);
		name2 = n.substr(index + 1, n.size() - index - 1);
		int index1 = -1;
		int index2 = -1;
		for (int i = 0; i < sites.size(); i++)
			if (name1 == sites.at(i).name)
			{
				name1 = "repeated";
				index1 = i;
				break;
			}
		for (int i = 0; i < sites.size(); i++)
			if (name2 == sites.at(i).name)
			{
				name2 = "repeated";
				index2 = i;
				break;
			}
		if (name1 != "repeated" && name2 != "repeated" && n.size() > 0) //both are new, n.size()>3 because it takes the last line
			//which is basically an empty string  
		{
			website newSite;
			website newSite2;
			newSite.name = name1;
			newSite2.name = name2;
			newSite.id = key;
			newSite2.pointedBy.push_back(key);
			key++;
			newSite2.id = key;
			newSite.pointTo.push_back(key);
			key++;
			sites.push_back(newSite);
			sites.push_back(newSite2);
		}
		else if (name1 != "repeated" && name2 == "repeated")
		{
			website newSite;
			newSite.name = name1;
			newSite.id = key;
			sites.at(index2).pointedBy.push_back(key);
			key++;
			newSite.pointTo.push_back(sites.at(index2).id);
			sites.push_back(newSite);

		}
		else if (name1 == "repeated" && name2 != "repeated")
		{
			website newSite2;
			newSite2.name = name2;
			newSite2.id = key;
			sites.at(index1).pointTo.push_back(key);
			key++;
			newSite2.pointedBy.push_back(sites.at(index1).id);
			sites.push_back(newSite2);
		}
		else if (name1 == "repeated" && name2 == "repeated")//both are repeated
		{
			sites.at(index1).pointTo.push_back(sites.at(index2).id);
			sites.at(index2).pointedBy.push_back(sites.at(index1).id);
		}
	}
	ifstream myfile2;
	myfile2.open("impressions.csv");
	ifstream clicksFile;
	clicksFile.open("clicks.csv");
	if (!myfile2.is_open()|| !clicksFile.is_open())
	{
		cout << "Error while opening the second or third files\n";
		return 0;
	}
	while (myfile2.good()&& clicksFile.good())
	{
		int imps;
		int clks;
		string n2;
		string name3;
		string name4;
		getline(myfile2, n);
		getline(clicksFile, n2);
		if (n.size() == 0||n2.size()==0) //to handle the case of empty string at the end
			break;
		int index = n.find(',');
		name1 = n.substr(0, index);
		name2 = n.substr(index + 1, n.size() - index - 1);
		name3 = n2.substr(0, index);
		name4 = n2.substr(index + 1, n2.size() - index - 1);
		stringstream obj(name2);
		obj >> imps;
		stringstream obj2(name4);
		obj2 >> clks;
		for (int i = 0; i < sites.size(); i++)
			if (name1 == sites.at(i).name)
			{
				sites.at(i).impressions = imps;
				sites.at(i).clicks = clks;
				break;
			}
	}
	myfile2.close();
	clicksFile.close();
	int countPush = 1;
	ifstream myfile3;
	myfile3.open("keywords.csv");
	while (myfile3.good())
	{
		getline(myfile3, n);
		int index = n.find(',');
		name1 = n.substr(0, index);
		name2 = n.substr(index + 1, n.size() - index - 1);
		int index3 = -2;
		for (int i = 0; i < sites.size(); i++)
			if (name1 == sites.at(i).name)
			{
				while (index3 != -1 &&name2.substr(0,10)!="ENDOFWORDS") {//last word of each website is to be set as END
					index3 = name2.find(',');
					sites.at(i).keywords.push_back(name2.substr(0, index3));
					name2 = name2.substr(index3 + 1, name2.size() - index3 - 1);
				}
				countPush = 1;
			}
	}


	//initialize a 2-D array to represent the graph
	//int** graph;
	//graph = new int* [sites.size()];
	//for (unsigned int i = 0; i < sites.size(); i++)
	//	graph[i] = new int[sites.size()];
	//for (unsigned int i = 0; i < sites.size(); i++)
	//	for (unsigned int j = 0; j < sites.size(); j++)
	//		graph[i][j] = 0;
	//for (unsigned int i = 0; i < sites.size(); i++)
	//	for (unsigned int j = 0; j < sites.at(i).pointTo.size(); j++)
	//	{
	//		int j2 = sites.at(i).pointTo.at(j);
	//		graph[i][j2] = 1;
	//	}
	//for (unsigned int i = 0; i < sites.size(); i++)
	//{
	//	for (unsigned int j = 0; j < sites.size(); j++)
	//		cout << graph[i][j] << "\t";
	//	cout << endl;
	//}

	double initial = 1.0 / sites.size();
	double* newRank;
	newRank = new double[sites.size()];
	double* Rank;
	Rank = new double[sites.size()];
	for (unsigned int i = 0; i < sites.size(); i++)
		newRank[i] = initial;
	//iteration 1
	for (unsigned int i = 0; i < sites.size(); i++)
	{
		double sum = 0.0;
		for (int j = 0; j < sites[i].pointedBy.size(); j++)
		{
			int s = sites[i].pointedBy[j];
			for (int k = 0; k < sites.size(); k++)
				if (s == sites[k].id)
				{
					sum += newRank[k] * 1.0 / sites[k].pointTo.size();
					break;
				}
		}
		Rank[i] = sum;
	}
	//iteration 1
	for (unsigned int i = 0; i < sites.size(); i++)
	{
		double sum = 0.0;
		for (int j = 0; j < sites[i].pointedBy.size(); j++)
		{
			int s = sites[i].pointedBy[j];
			for (int k = 0; k < sites.size(); k++)
				if (s == sites[k].id)
				{
					sum += Rank[k] * 1.0 / sites[k].pointTo.size();
					break;
				}
		}
		newRank[i] = sum;
	}
	struct ranking {
		double rankValue;
		int index;
	};
	ranking tempRanking;
	vector<ranking> rankIndex;
	//sorting the rank
	for (unsigned int i = 0; i < sites.size(); i++)
	{
		double min = newRank[0];
		int minIndex = 0;
		for (int j = 0; j < sites.size(); j++)
			if (newRank[j] < min)
			{
				min = newRank[j];
				minIndex = j;
			}
		tempRanking.rankValue = newRank[minIndex];
		tempRanking.index = minIndex;
		rankIndex.push_back(tempRanking);
		//sites[maxIndex].pageRank = i+1; //i+1 instead of rankP++
		newRank[minIndex] = 99999;
	}
	//solving the problem of equal ranks
	for (int i = 0; i < sites.size(); i++)
	{
		sites[(rankIndex[i].index)].pageRank = rankP;
		for (i; i != sites.size() - 1 && rankIndex[i].rankValue == rankIndex[i + 1].rankValue; i++)
			sites[i + 1].pageRank = rankP;

		rankP++;
	}

	for (int i = 0; i < sites.size(); i++)
	{
		if (sites[i].impressions == 0)
			sites[i].CTR = 0;
		else
			sites[i].CTR = 1.0 * sites[i].clicks / sites[i].impressions;
	}
	for (int i = 0; i < sites.size(); i++)
		sites[i].score = (0.4 * sites[i].pageRank) + 0.6 * (((1 - ((0.1 * sites[i].impressions) / (1 + (0.1 * sites[i].impressions)))) * sites[i].pageRank) + (((0.1 * sites[i].impressions) / (1 + (0.1 * sites[i].impressions))) * sites[i].CTR));
	cout << "Welcome!\n";
	cout << "What would you like to do?\n";
	cout << "1. New search\n";
	cout << "2. Exit\n";
	int in;
	int inputReSearch = 0;
	string input;
	vector<string> words;
	vector<website> results;
	cin >> in;
	do {
		if (in == 1 || inputReSearch == 2)
		{
			cout << "Enter your search query\n";
			cin.ignore();
			getline(cin, input);
			results.clear();
		}
		else
		{
			return 0;
		}
		//assuming no invalid input like only one quotationa or searching with "or" or "and" instead of "OR" and "AND"
		//case 1: Quotations
		ofstream impsOpen;
		impsOpen.open("impressions.csv");
		ofstream clksOpen;
		clksOpen.open("clicks.csv");
		if (input[0] == '"' && input[input.size() - 1] == '"')
		{
			input.erase(0, 1);
			input.erase(input.size() - 1, 1);
			//call the search function of quotations which calls the split function at first
			words = split(input);
			for (int j = 0; j < sites.size(); j++)
			{
				for (int i = 0; i < words.size(); i++)
					for (int k = 0; k < sites[j].keywords.size(); k++)
					{
						int counter = 0;
						if (words[i] == sites[j].keywords[k])
						{
							counter++;
							for (i, k; i < words.size() - 1; i++, k++)
								if (words[i + 1] == sites[j].keywords[k + 1])
									counter++;
						}
						if (counter == words.size())
						{
							results.push_back(sites[j]);
							sites[j].impressions++;
						}
					}
			}
		}
		else if (input.find("AND") != -1)
		{
			do {
				input.erase(input.find("AND"), 4);
				//call the search function of ORed string which calls the split function at first
				words = split(input);
				for (int j = 0; j < sites.size(); j++)
				{
					for (int i = 0; i < words.size(); i++)
					{
						int counter = 0;
						for (int k = 0; k < sites[j].keywords.size(); k++)
						{
							if (words[i] == sites[j].keywords[k])
							{
								counter++;
								for (i, k; i < words.size() - 1; i++, k++)
									if (words[i + 1] == sites[j].keywords[k + 1])
										counter++;
							}

						}
						if (counter == words.size())
						{
							results.push_back(sites[j]);
							sites[j].impressions++;
						}
					}
				}
			} while (input.find("AND") != -1);
		}
		else {
			if (input.find("OR") != -1)
			{
				do {
					input.erase(input.find("OR"), 3);
				} while (input.find("OR") != -1);
			}
			//call the search function of ORed string which calls the split function at first
			words = split(input);
			bool stop; 
			for (int j = 0; j < sites.size(); j++)
			{
				stop = 0;
				for (int i = 0; i < words.size() && !stop; i++)
					for (int k = 0; k < sites[j].keywords.size() && !stop; k++)
						if (words[i] == sites[j].keywords[k])
						{
							results.push_back(sites[j]);
							sites[j].impressions++;
							stop = 1;
							break;
						}
			}
		}

		//sorting the results by score
		for (unsigned int i = 0; i < results.size()-1; i++)
		{
			double max = results[i].score;
			int maxIndex = i;
			for (int j = i+1; j < results.size(); j++)
				if (results[j].score > max)
					maxIndex = j;
			website temp = results[i];
			results[i] = results[maxIndex];
			results[maxIndex] = temp;
		}
		
		do {
			cout << "Search results:\n";
			for (int i = 0; i < results.size(); i++)
			{
				cout << i + 1 << ".\t" << results[i].name << endl;
			}
			cout << endl;
			cout << "Would you like to\n";
			cout << "1.\tChoose a webpage to open\n";
			cout << "2.\tNew search\n";
			cout << "3.\tExit\n";
			cout << "\nType in your choice: ";
			cin >> inputReSearch;
			if (inputReSearch==3)
			{
				for (int i = 0; i < sites.size(); i++)
				{
					impsOpen << sites[i].name << "," << sites[i].impressions << endl;
					clksOpen << sites[i].name << "," << sites[i].clicks << endl;
				}
				impsOpen.close();
				clksOpen.close();
				return 0;
			}
			if (inputReSearch == 1)
			{
				cout << "Enter the number of the website\n";
				cin >> inputReSearch;
				while (inputReSearch > results.size() + 1 || inputReSearch < 1)
				{
					cout << "Invalide input. Please enter the number of the website\n";
					cin >> inputReSearch;
				}
				cout << "You're now viewing " << results[inputReSearch - 1].name << "." << endl;
				for (int i = 0; i < sites.size(); i++)
					if (results[inputReSearch - 1].id == sites[i].id)
					{
						sites[i].clicks++;
						break;
					}
				cout << "Would you like to\n";
				cout << "1.\tBack to search results\n";
				cout << "2.\tNew search\n";
				cout << "3.\tExit\n";
				cout << "\nType in your choice: ";
				
				cin >> inputReSearch;
				while (inputReSearch > 3 || inputReSearch < 0)
				{
					cout << "Invalid input. Re-enter a number from 1 - 3\n";
				}
				if (inputReSearch == 3)
				{
					for (int i = 0; i < sites.size(); i++)
					{
						impsOpen << sites[i].name << "," << sites[i].impressions<<endl;
						clksOpen << sites[i].name << "," << sites[i].clicks<<endl;
					}
					impsOpen.close();
					clksOpen.close();
					return 0;
				}
			}
		} while (inputReSearch == 1);
	} while (inputReSearch == 2);	
	return 0;
}
vector<string> split(string input) {
	string temp;
	vector<string> words;
	istringstream ss(input);
	while (ss >> temp)
		words.push_back(temp);
	return words;
}
	