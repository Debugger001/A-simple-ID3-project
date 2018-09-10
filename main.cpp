//
//  main.cpp
//  CS214_Project3
//
//  Created by LC on 2018/4/23.
//  Copyright © 2018年 LC. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>

using namespace std;

const int resultsrange = 5;
vector<string> attrb;
vector<vector<string>> details;
vector<vector<string>> Datum;
vector<vector<string>> FullDatum;
vector<string> age;
vector<string> prescript;
vector<string> astigmatic;
vector<string> tearrate;
vector<string> conclusion;
vector<string> Fullage;
vector<string> Fullprescript;
vector<string> Fullastigmatic;
vector<string> Fulltearrate;
vector<string> Fullconclusion;
bool isCorrect = false;


bool IsMeet(int i, vector<string> results, vector<vector<string>> Datum)
{
    bool isMeet = true;
    for (int j = 0; j < results.size(); j++)
    {
        if (results[j] != " ")
        {
            if (results[j] == Datum[j][i])
                continue;
            else
            {
                isMeet = false;
                break;
            }
        }
        else if (results[j] == " ")
            continue;
    }
    return isMeet;
}

float Info_Entropy(string concls, vector<string> results, vector<vector<string>> details, vector<vector<string>> Datum, vector<string> attrb)
{
    float info_entropy = 0;
    int ammounts[resultsrange];
    vector<int> conclsctgy;
    for (int i = 0; i < resultsrange; i++)
        ammounts[i] = 0;
    int num = 0;
    int totalnum = 0;
    for (int i = 0; i < attrb.size(); i++)
        if (attrb[i] == concls)
            num = i;
    for (int i = 0; i < Datum[num].size(); i++)
        if (IsMeet(i, results, Datum))
            for (int j = 0; j < details[num].size(); j++)
                if (details[num][j] == Datum[num][i])
                    ammounts[j]++;
    for (int i = 0; i < resultsrange; i++)
        totalnum += ammounts[i];
    for (int i = 0; i < resultsrange; i++)
        if (ammounts[i] != 0)
            info_entropy += -(ammounts[i]/(totalnum+0.0))*log2f((ammounts[i]/(totalnum+0.0)));
    return info_entropy;
}

float Prob(string attr, string result, string concls, vector<string> results, vector<vector<string>> details, vector<vector<string>> Datum, vector<string> attrb)
{
    float probability = 0;
    int num = 0;
    int totalnum = 0;
    int ammounts[resultsrange];
    for (int i = 0; i < resultsrange; i++)
        ammounts[i] = 0;
    for (int i = 0; i < attrb.size(); i++)
        if (attrb[i] == attr)
            num = i;
    for (int i = 0; i < Datum[num].size(); i++)
        if (IsMeet(i, results, Datum))
            for (int j = 0; j < details[num].size(); j++)
                if (details[num][j] == Datum[num][i])
                    ammounts[j]++;
    for (int i = 0; i < resultsrange; i++)
        totalnum += ammounts[i];
    for (int i = 0; i < details[num].size(); i++)
        if (details[num][i] == result)
            probability = ammounts[i] / (totalnum + 0.0);
    return probability;
}

float Condit_Info_Entropy(string attr, string concls, vector<string> results, vector<vector<string>> details, vector<string> attrb)
{
    float condit_info_entropy = 0;
    float probability = 0;
    int num = 0;
    for (int i = 0; i < attrb.size(); i++)
        if (attrb[i] == attr)
            num = i;
    for (int i = 0; i < details[num].size(); i++)
    {
        probability = Prob(attr, details[num][i], concls, results, details, Datum, attrb);
        results[num] = details[num][i];
        condit_info_entropy += probability * Info_Entropy(concls, results, details, Datum, attrb);
        results[num] = " ";
    }
    return condit_info_entropy;
}

float Info_Gain(string attr, string concls, vector<string> results, vector<vector<string>> details, vector<vector<string>> Datum, vector<string> attrb)
{
    float info_gain = 0;
    info_gain = Info_Entropy(concls, results, details, Datum, attrb) - Condit_Info_Entropy(attr, concls, results, details, attrb);
    return info_gain;
}

bool IsRecurse(vector<string> results, vector<vector<string>> Datum)
{
    bool isRecurse = false;
    for (int k = 0; k < results.size(); k++)
        if (results[k] == " ")
            isRecurse = true;
    vector<string> meetdatum;
    meetdatum.empty();
    bool isIn = false;
    for (int i = 0; i < Datum[0].size(); i++)
    {
        isIn = false;
        if (IsMeet(i, results, Datum))
        {
            for (int j = 0; j < meetdatum.size(); j++)
            {
                if (Datum[details.size() - 1][i] == meetdatum[j])
                {
                    isIn = true;
                    break;
                }
            }
            if (!isIn)
                meetdatum.push_back(Datum[details.size() - 1][i]);
        }
    }
    /*
    cout << endl;
    for (int i = 0; i < meetdatum.size(); i++)
        cout << meetdatum[i] << endl;
    cout << meetdatum.size() << endl;
    */
    if (meetdatum.size() > 1)
        isRecurse = true;
    else
        isRecurse = false;
    return isRecurse;
}


string Decision_Tree(string concls, vector<string> results, vector<vector<string>> details, vector<vector<string>> Datum, vector<string> attrb)
{
    string feedback = "";
    int indents = 0;
    float gain[resultsrange];
    int order[resultsrange];

    for (int i = 0; i < resultsrange; i++)
    {
        gain[i] = 0;
        order[i] = i;
    }
    if (IsRecurse(results, Datum))
    {
        for (int i = 0; i < results.size(); i++)
            if (results[i] == " ")
                gain[i] = Info_Gain(attrb[i], concls, results, details, Datum, attrb);
        //cout << "Gain ";
        //for (int i = 0; i < results.size(); i++)
        //    cout << gain[i] << " ";
        //cout << endl;
        for (int i = 0; i < results.size(); i++)
        {
            for (int j = i; j < results.size(); j++)
            {
                if (gain[i] < gain[j])
                {
                    float temp = gain[j];
                    gain[j] = gain[i];
                    gain[i] = temp;
                    order[j] = i;
                    order[i] = j;
                }
            }
        }
        cout << "check " << attrb[order[0]] << ":" << endl;
        for (int i = 0; i < details[order[0]].size(); i++)
        {
            results[order[0]] = details[order[0]][i];
            indents = 0;
            for (int i = 0; i < results.size(); i++)
                if (results[i] != " ")
                    indents++;
            for (int i = 0; i < indents; i++)
                cout << "  ";
            cout << "If " << details[order[0]][i] << " then " << Decision_Tree(concls, results, details, Datum, attrb) << endl;
        }
    }
    else
    {
        string conc = "";
        for (int i = 0; i < Datum[0].size(); i++)
            if (IsMeet(i, results, Datum))
            {
                conc = Datum[details.size() - 1][i];
                break;
            }
        feedback = "draw conclusion (" + conc + "). ";
    }
    return feedback;
}

bool Prediction(string actualconcls, string concls, vector<string> results, vector<string> actualresults, vector<vector<string>> details, vector<vector<string>> Datum)
{
    string conclusion = "";
    float gain[resultsrange];
    int order[resultsrange];
    
    for (int i = 0; i < resultsrange; i++)
    {
        gain[i] = 0;
        order[i] = i;
    }
    if (IsRecurse(results, Datum))
    {
        for (int i = 0; i < results.size(); i++)
            if (results[i] == " ")
                gain[i] = Info_Gain(attrb[i], concls, results, details, Datum, attrb);
        //cout << "Gain ";
        //for (int i = 0; i < results.size(); i++)
        //    cout << gain[i] << " ";
        //cout << endl;
        for (int i = 0; i < results.size(); i++)
        {
            for (int j = i; j < results.size(); j++)
            {
                if (gain[i] < gain[j])
                {
                    float temp = gain[j];
                    gain[j] = gain[i];
                    gain[i] = temp;
                    order[j] = i;
                    order[i] = j;
                }
            }
        }
        for (int i = 0; i < details[order[0]].size(); i++)
        {
            if (details[order[0]][i] == actualresults[order[0]])
                results[order[0]] = actualresults[order[0]];
        }
        Prediction(actualconcls, concls, results, actualresults, details, Datum);
    }
    else
    {
        string conc = "";
        for (int i = 0; i < Datum[0].size(); i++)
            if (IsMeet(i, results, Datum))
            {
                conc = Datum[details.size() - 1][i];
                break;
            }
        conclusion = "Conclusion: " + conc + ".";
        if (actualconcls == conc)
            isCorrect = true;
        else
            isCorrect = false;
    }
    return isCorrect;
}


int main()
{
    Datum.push_back(age);
    Datum.push_back(prescript);
    Datum.push_back(astigmatic);
    Datum.push_back(tearrate);
    Datum.push_back(conclusion);
    details.push_back(age);
    details.push_back(prescript);
    details.push_back(astigmatic);
    details.push_back(tearrate);
    details.push_back(conclusion);
    FILE *pFile;
    pFile = fopen("CS214_Project3_2.txt", "r+");
    char att[20];
    char datum[20];
    for (int i = 0; i < 5; i++)
    {
        fscanf(pFile, "%s", att);
        attrb.push_back(att);
    }
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            fscanf(pFile, "%s", datum);
            Datum[j].push_back(datum);
        }
    }
    for (int i = 0; i < 5; i++)
    {
        bool isin;
        for (int j = 0; j < Datum[i].size(); j++)
        {
            isin = false;
            if (details[i].size() == 0)
                details[i].push_back(Datum[i][0]);
            for (int k = 0; k < details[i].size(); k++)
                if (Datum[i][j] == details[i][k])
                    isin = true;
            if (! isin)
                details[i].push_back(Datum[i][j]);

        }
    }
    fclose(pFile);
    /************ Test details
    for (int i = 0; i < details.size(); i++)
    {
        for (int j = 0; j < details[i].size(); j++)
            cout << details[i][j] << " ";
        cout << endl;
    }
     */
    

    /*
    float f = Info_Entropy("conclusion", Datum, attrb);
    float c = Condit_Info_Entropy("tearrate", "conclusion", Datum, attrb);
    float g = Info_Gain("tearrate", "conclusion", Datum, attrb);
    
    vector<string> InTree;
    vector<string> DecisionTree = Decision_Tree(InTree, "conclusion", Datum, attrb);
    //cout << f << endl << c << endl << g << endl;
    for (int i = 0; i < DecisionTree.size(); i++)
        cout << DecisionTree[i] << endl;
    */
    
    vector<string> results;
    results.push_back(" ");
    results.push_back(" ");
    results.push_back(" ");
    results.push_back(" ");
    vector<string> actualresults;
    actualresults.push_back(" ");
    actualresults.push_back(" ");
    actualresults.push_back(" ");
    actualresults.push_back(" ");
    //cout << Info_Entropy("conclusion", results, details, Datum, attrb) << endl;
    //cout << Condit_Info_Entropy("age", "conclusion", results, details, attrb) << endl;
    Decision_Tree("conclusion", results, details, Datum, attrb);
    FILE *pFilef;
    pFilef = fopen("CS214_Projec3_Full_Data_Set.txt", "r+");
    vector<vector<string>> FullDatum;
    FullDatum.push_back(Fullage);
    FullDatum.push_back(Fullprescript);
    FullDatum.push_back(Fullastigmatic);
    FullDatum.push_back(Fulltearrate);
    FullDatum.push_back(Fullconclusion);
    int correctnum = 0;
    for (int ii = 0; ii < 24; ii++)
    {
        for (int jj = 0; jj < 5; jj++)
        {
            fscanf(pFile, "%s", datum);
            FullDatum[jj].push_back(datum);
            if (jj < 4)
                actualresults[jj] = datum;
        }
        string actualconcls = FullDatum[attrb.size() - 1][ii];
        /*
        for (int i = 0; i < results.size(); i++)
        {
            cout << "Please input the result of " << attrb[i] << ", ";
            for (int j = 0; j < details[i].size(); j++)
            {
                cout << details[i][j];
                if (j < details[i].size() - 1)
                    cout << " / ";
                else
                    cout << "?" << endl;
            }
            cin >> actualresult;
            actualresults[i] = actualresult;
        }
        */
        if (Prediction(actualconcls, "conclusion", results, actualresults, details, Datum))
            correctnum++;
    }
    fclose(pFilef);
    float correctratio = 0;
    correctratio = correctnum / (24 + 0.0);
    cout << endl << "The ratio of correct prediction for the full data set is: ";
    cout << correctratio * 100 << "%" << endl;
    return 0;
}




/*
 float Info_Entropy(string attr, vector<vector<string>> Datum, vector<string> attrb)
 {
 float info_entropy = 0;
 int ammounts[resultsrange];
 for (int i = 0; i < 30; i++)
 ammounts[i] = 0;
 vector<string> results;
 int num = 0;
 int totalnum = 0;
 for (int i = 0; i < attrb.size(); i++)
 if (attrb[i] == attr)
 num = i;
 bool isin = false;
 for (int i = 0; i < Datum[num].size(); i++)
 {
 isin = false;
 if (results.size() == 0)
 {
 results.push_back(Datum[num][i]);
 }
 for (int j = 0; j < results.size(); j++)
 {
 if (Datum[num][i] == results[j])
 {
 ammounts[j]++;
 isin = true;
 }
 }
 if (! isin)
 {
 results.push_back(Datum[num][i]);
 ammounts[results.size() - 1]++;
 }
 }
 for (int i = 0; i < resultsrange; i++)
 totalnum += ammounts[i];
 for (int i = 0; i < resultsrange; i++)
 if (ammounts[i] != 0)
 info_entropy += - ammounts[i] / (totalnum + 0.0) * log2f(ammounts[i] / (totalnum + 0.0));
 return info_entropy;
 }
 
 float Condit_Info_Entropy(string attr, string concls, vector<vector<string>> Datum, vector<string> attrb)
 {
 float condit_info_entropy = 0;
 vector<string> results;
 int attrammounts[resultsrange];
 int totalnum = 0;
 vector<int> conclsammounts;
 for (int i = 0; i < resultsrange; i++)
 {
 attrammounts[i] = 0;
 conclsammounts.push_back(0);
 }
 int attrnum = 0, conclsnum = 0;
 for (int i = 0; i < attrb.size(); i++)
 {
 if (attrb[i] == attr)
 attrnum = i;
 else if (attrb[i] == concls)
 conclsnum = i;
 }
 bool isin = false;
 for (int i = 0; i < Datum[attrnum].size(); i++)
 {
 isin = false;
 if (results.size() == 0)
 results.push_back(Datum[attrnum][i]);
 for (int j = 0; j < results.size(); j++)
 {
 if (Datum[attrnum][i] == results[j])
 {
 attrammounts[j]++;
 isin = true;
 }
 }
 if (! isin)
 {
 results.push_back(Datum[attrnum][i]);
 attrammounts[results.size() - 1]++;
 }
 }
 vector<string> conclsctgy;
 for (int i = 0; i < Datum[conclsnum].size(); i++)
 {
 isin = false;
 if (conclsctgy.size() == 0)
 conclsctgy.push_back(Datum[conclsnum][i]);
 for (int j = 0; j < conclsctgy.size(); j++)
 if (Datum[conclsnum][i] == conclsctgy[j])
 isin = true;
 if (! isin)
 conclsctgy.push_back(Datum[conclsnum][i]);
 }
 vector<vector<int>> statistics;
 for (int i = 0; i < results.size(); i++)
 statistics.push_back(conclsammounts);
 for (int i = 0; i < Datum[conclsnum].size(); i++)
 for (int j = 0; j < conclsctgy.size(); j++)
 if (conclsctgy[j] == Datum[conclsnum][i])
 for (int k = 0; k < results.size(); k++)
 if (Datum[attrnum][i] == results[k])
 statistics[k][j]++;
 for (int i = 0; i < resultsrange; i++)
 totalnum += attrammounts[i];
 for (int i = 0; i < statistics.size(); i ++)
 {
 int condittotal = 0;
 float condit_entropy = 0;
 for (int j = 0; j < statistics[i].size(); j++)
 condittotal += statistics[i][j];
 for (int j = 0; j < statistics[i].size(); j++)
 if (statistics[i][j] != 0)
 condit_entropy += -statistics[i][j]/(condittotal+0.0)*log2f(statistics[i][j]/(condittotal+0.0));
 condit_info_entropy += attrammounts[i]/(totalnum+0.0)*condit_entropy;
 }
 return condit_info_entropy;
 }
 
 float Info_Gain(string attr, string result, vector<vector<string>> Datum, vector<string> attrb)
 {
 float info_gain = 0;
 info_gain = Info_Entropy(result, Datum, attrb) - Condit_Info_Entropy(attr, result, Datum, attrb);
 return info_gain;
 }
 
 vector<string> Sort_Attrb(vector<string> InTree, string concls, vector<vector<string>> Datum, vector<string> attrb)
 {
 vector<string> sorted_attrb;
 float gain[resultsrange];
 for (int i = 0; i < resultsrange; i++)
 gain[i] = 0;
 
 for (int i = 1; i < attrb.size(); i++)
 {
 if (attrb[i] != concls)
 gain[i] = Info_Gain(attrb[i], concls, Datum, attrb);
 }
 float gaincopy[resultsrange];
 for (int i = 0; i < resultsrange; i++)
 gaincopy[i] = gain[i];
 for (int i = 1; i < resultsrange; i++)
 {
 for (int j = i; j < resultsrange; j++)
 {
 if (gain[i] < gain[j])
 {
 float temp = gain[i];
 gain[i] = gain[j];
 gain[j] = temp;
 }
 }
 }
 bool isinTree = false;
 for (int i = 1; i < attrb.size() - 1; i++)
 {
 cout << gain[i];
 for (int j = 0; j < resultsrange; j++)
 if (gain[i] == gaincopy[j])
 {
 for (int k = 0; k < InTree.size(); k++)
 if (attrb[j] == InTree[k])
 isinTree = true;
 if (! isinTree)
 {
 sorted_attrb.push_back(attrb[j]);
 cout << attrb[j] << endl;
 break;
 }
 }
 }
 return sorted_attrb;
 }
 
 vector<string> Decision_Tree(vector<string> Intree, string concls, vector<vector<string>> Datum, vector<string> attrb)
 {
 vector<string> decision_tree;
 vector<string> NextDecisions;
 if (Intree.size() < attrb.size() - 2)
 {
 NextDecisions = Sort_Attrb(Intree, concls, Datum, attrb);
 Intree.push_back(NextDecisions[0]);
 decision_tree = Decision_Tree(Intree, concls, Datum, attrb);
 }
 return Intree;
 }
 
 */




















