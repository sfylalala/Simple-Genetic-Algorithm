#pragma once
/*
*A Simple Class for Genetic Algorithm
*only needed to write three functions to work
*welcome improvement
*Author: Sfylalala~_~
*Date: 2018/11/21
*/
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<algorithm>
#include<functional>
#include<map>
#include<iostream>

using std::pair; using std::vector; using std::function; using std::make_pair; using std::sort; using std::cerr; using std::endl;

namespace Genetic_Alg {

	void init_srand() {
		srand(time(NULL));
	}

	//return value in [i,j)
	int random(int i, int j) {
		return rand() % (j - i) + i;
	}

	template<class individual>
	class Genetic {
	public:
		Genetic() :vRating(0), toRelease(true) {};
		~Genetic() {
			if (toRelease == true) {
				for (auto& i : population) {
					delete i.first;
				}
			}
		}

	public:
		int size() { return population.size(); };
		vector<pair<individual*, double>>* getPopulation() { return &population; }
	public:
		void createPopulation(int size);
		void eliminate(int num);
		void evoluton(int num);
	public:
		void setGenerateFunc(function<individual*()> func, bool isToRelease = true) {
			generateFunc = func;
			toRelease = isToRelease;
		}
		void setValueFunc(function<double(const individual*)> func) {
			valueFunc = func;
		}
		void setMatingFunc(function<individual*(const individual* a, const individual* b)> func) {
			matingFunc = func;
		}
		void setVariationFunc(function<void(individual*)> func, int rating) {
			variationFunc = func;
			vRating = rating;
		}
	private:
		//sort the population vector by value
		void mySort() {
			sort(population.begin(), population.end(), [](pair<individual*, double> a, pair<individual*, double> b) {return a.second > b.second; });
		}

		//use roulette to choose an individual
		//double sum: return value of valueSum
		//return: the rank of the selected individual 
		int roulette(double sum) {
			double n = rand() / (RAND_MAX / sum);
			int i = 0;
			while (n > population[i].second) {
				n -= population[i].second;
				i++;
			}
			return i;
		}

		//return: the sum of all individual's value
		double valueSum() {
			double sum = 0;
			for (auto& i : population) {
				sum += i.second;
			}
			return sum;
		}
	private:
		vector<pair<individual*, double>> population;
		function<individual*()> generateFunc; //the function generates an individual
		function<double(const individual*)> valueFunc; //the function values an individual
		function<individual*(const individual* a, const individual* b)> matingFunc; //the function mates two indiviudal to create a new individual
		function<void(individual*)> variationFunc; //the function varies an individual
		bool toRelease; //whether the individual needed to be deleted
		int vRating; //the rating of variation
	};

	//create a population
	//int size:the size of the population
	template<class individual>
	void Genetic<individual>::createPopulation(int size) {
		for (int i = 0; i < size; i++) {
			auto m = generateFunc();
			population.push_back(make_pair(m, valueFunc(m)));
		}
	}

	//eliminate some of the worst individuals in the population
	//int num: the number of individuals to be eliminated
	template<class individual>
	void Genetic<individual>::eliminate(int num) {
		if (num > size()) {
			cerr << "Num Is Bigger Than Size!" << endl;
		}
		else {
			individual* p = nullptr;
			mySort();
			if (toRelease) {
				for (int i = 0; i < num; i++) {
					delete (*population.end()).first;
					population.pop_back();
				}
			}
			else {
				population.erase(population.end() - num, population.end());
			}
		}
	}

	//add some individuals and eliminate the same number
	//num: the number of individuals to be added
	template<class individual>
	void Genetic<individual>::evoluton(int num) {
		mySort();
		double sum = valueSum();
		for (int i = 0; i < num; i++) {
			auto m = matingFunc(population[roulette(sum)].first, population[roulette(sum)].first);
			if (variationFunc != 0 && rand() / RAND_MAX < vRating) {
				variationFunc(m);
			}
			population.push_back(make_pair(m, valueFunc(m)));
		}
		eliminate(num);
	}

}