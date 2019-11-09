//
//  main.cpp
//  blender2opengles
//
//  Created by RRC on 8/12/13.
//  Copyright (c) 2013 Ricardo Rendon Cepeda. All rights reserved.
//

// C++ Standard Library
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Model Structure
typedef struct Model
{
    int vertices;
    int positions;
    int texels;
    int normals;
    int faces;
    int materials;
}
Model;

Model getOBJinfo(string fp)
{
    Model model = {0};
    
    // Open OBJ file
    ifstream inOBJ;
    inOBJ.open(fp);
    if(!inOBJ.good())
    {
        cout << "ERROR OPENING OBJ FILE" << endl;
        exit(1);
    }
    
    // Read OBJ file
    while(!inOBJ.eof())
    {
        string line;
        getline(inOBJ, line);
        string type = line.substr(0,2);
        
        if(type.compare("v ") == 0)
            model.positions++;
        else if(type.compare("vt") == 0)
            model.texels++;
        else if(type.compare("vn") == 0)
            model.normals++;
        else if(type.compare("f ") == 0)
            model.faces++;
    }
    
    model.vertices = model.faces*3;
    
    // Close OBJ file
    inOBJ.close();
    
    return model;
}

void extractOBJdata(string fp, float positions[][3], float texels[][2], float normals[][3], int faces[][10], string* materials, int m)
{
    // Counters
    int p = 0;
    int t = 0;
    int n = 0;
    int f = 0;
    
    // Index
    int mtl = 0;
    
    // Open OBJ file
    ifstream inOBJ;
    inOBJ.open(fp);
    if(!inOBJ.good())
    {
        cout << "ERROR OPENING OBJ FILE" << endl;
        exit(1);
    }
    
    // Read OBJ file
    while(!inOBJ.eof())
    {
        string line;
        getline(inOBJ, line);
        string type = line.substr(0,2);
        
        // Material
        if(type.compare("us") == 0)
        {
            // Extract token
            string l = "usemtl ";
            string material = line.substr(l.size());
            
            for(int i=0; i<m; i++)
            {
                if(material.compare(materials[i]) == 0)
                    mtl = i;
            }
        }
        
        // Positions
        if(type.compare("v ") == 0)
        {
            // Copy line for parsing
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            // Extract tokens
            strtok(l, " ");
            for(int i=0; i<3; i++)
                positions[p][i] = atof(strtok(NULL, " "));
            
            // Wrap up
            delete[] l;
            p++;
        }
        
        // Texels
        else if(type.compare("vt") == 0)
        {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            for(int i=0; i<2; i++)
                texels[t][i] = atof(strtok(NULL, " "));
            
            delete[] l;
            t++;
        }
        
        // Normals
        else if(type.compare("vn") == 0)
        {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            for(int i=0; i<3; i++)
                normals[n][i] = atof(strtok(NULL, " "));
            
            delete[] l;
            n++;
        }
        
        // Faces
        else if(type.compare("f ") == 0)
        {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            for(int i=0; i<9; i++)
                faces[f][i] = atof(strtok(NULL, " /"));
            
            // Append material
            faces[f][9] = mtl;
            
            delete[] l;
            f++;
        }
    }
    
    // Close OBJ file
    inOBJ.close();
}

void writeH(string fp, string name, Model model)
{
    // Create H file
    ofstream outH;
    outH.open(fp);
    if(!outH.good())
    {
        cout << "ERROR CREATING H FILE" << endl;
        exit(1);
    }
    
    // Write to H file
    outH << "// This is a .h file for the model: " << name << endl;
    outH << endl;
    
    // Write statistics
    outH << "// Positions: " << model.positions << endl;
    outH << "// Texels: " << model.texels << endl;
    outH << "// Normals: " << model.normals << endl;
    outH << "// Faces: " << model.faces << endl;
    outH << "// Vertices: " << model.vertices << endl;
    outH << "// Materials: " << model.materials << endl;
    outH << endl;
    
    // Write declarations
    outH << "const int " << name << "Vertices;" << endl;
    outH << "const float " << name << "Positions[" << model.vertices*3 << "];" << endl;
    outH << "const float " << name << "Texels[" << model.vertices*2 << "];" << endl;
    outH << "const float " << name << "Normals[" << model.vertices*3 << "];" << endl;
    outH << endl;
    
    outH << "const int " << name << "Materials;" << endl;
    outH << "const int " << name << "Firsts[" << model.materials << "];" << endl;
    outH << "const int " << name << "Counts[" << model.materials << "];" << endl;
    outH << endl;
    
    outH << "const float " << name << "Diffuses[" << model.materials << "]" << "[" << 3 << "];" << endl;
    outH << "const float " << name << "Speculars[" << model.materials << "]" << "[" << 3 << "];" << endl;
    outH << endl;
    
    // Close H file
    outH.close();
}

void writeCvertices(string fp, string name, Model model)
{
    // Create C file
    ofstream outC;
    outC.open(fp);
    if(!outC.good())
    {
        cout << "ERROR CREATING C FILE" << endl;
        exit(1);
    }
    
    // Write to C file
    outC << "// This is a .c file for the model: " << name << endl;
    outC << endl;
    
    // Header
    outC << "#include " << "\"" << name << ".h" << "\"" << endl;
    outC << endl;
    
    // Vertices
    outC << "const int " << name << "Vertices = " << model.vertices << ";" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

void writeCpositions(string fp, string name, Model model, int faces[][10], float positions[][3], int counts[])
{    
    // Append C file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Positions
    outC << "const float " << name << "Positions[" << model.vertices*3 << "] = " << endl;
    outC << "{" << endl;
    for(int j=0; j<model.materials; j++)
    {
        counts[j] = 0;
        
        for(int i=0; i<model.faces; i++)
        {
            if(faces[i][9] == j)
            {
                int vA = faces[i][0] - 1;
                int vB = faces[i][3] - 1;
                int vC = faces[i][6] - 1;
                
                outC << positions[vA][0] << ", " << positions[vA][1] << ", " << positions[vA][2] << ", " << endl;
                outC << positions[vB][0] << ", " << positions[vB][1] << ", " << positions[vB][2] << ", " << endl;
                outC << positions[vC][0] << ", " << positions[vC][1] << ", " << positions[vC][2] << ", " << endl;
                
                counts[j] += 3;
            }
        }
    }
    outC << "};" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

void writeCtexels(string fp, string name, Model model, int faces[][10], float texels[][2])
{
    // Append C file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Texels
    outC << "const float " << name << "Texels[" << model.vertices*2 << "] = " << endl;
    outC << "{" << endl;
    for(int j=0; j<model.materials; j++)
    {
        for(int i=0; i<model.faces; i++)
        {
            if(faces[i][9] == j)
            {
                int vtA = faces[i][1] - 1;
                int vtB = faces[i][4] - 1;
                int vtC = faces[i][7] - 1;
                
                outC << texels[vtA][0] << ", " << texels[vtA][1] << ", " << endl;
                outC << texels[vtB][0] << ", " << texels[vtB][1] << ", " << endl;
                outC << texels[vtC][0] << ", " << texels[vtC][1] << ", " << endl;
            }
        }
    }
    outC << "};" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

void writeCnormals(string fp, string name, Model model, int faces[][10], float normals[][3])
{
    // Append C file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Normals
    outC << "const float " << name << "Normals[" << model.vertices*3 << "] = " << endl;
    outC << "{" << endl;
    for(int j=0; j<model.materials; j++)
    {
        for(int i=0; i<model.faces; i++)
        {
            if(faces[i][9] == j)
            {
                int vnA = faces[i][2] - 1;
                int vnB = faces[i][5] - 1;
                int vnC = faces[i][8] - 1;
                
                outC << normals[vnA][0] << ", " << normals[vnA][1] << ", " << normals[vnA][2] << ", " << endl;
                outC << normals[vnB][0] << ", " << normals[vnB][1] << ", " << normals[vnB][2] << ", " << endl;
                outC << normals[vnC][0] << ", " << normals[vnC][1] << ", " << normals[vnC][2] << ", " << endl;
            }
        }
    }
    outC << "};" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

int getMTLinfo(string fp)
{
    int m = 0;
    
    // Open MTL file
    ifstream inMTL;
    inMTL.open(fp);
    if(!inMTL.good())
    {
        cout << "ERROR OPENING MTL FILE" << endl;
        exit(1);
    }
    
    // Read MTL file
    while(!inMTL.eof())
    {
        string line;
        getline(inMTL, line);
        string type = line.substr(0,2);
        
        if(type.compare("ne") == 0)
            m++;
    }
    
    // Close MTL file
    inMTL.close();
    
    return m;
}

void extractMTLdata(string fp, string* materials, float diffuses[][3], float speculars[][3])
{
    // Counters
    int m = 0;
    int d = 0;
    int s = 0;
    
    // Open MTL file
    ifstream inMTL;
    inMTL.open(fp);
    if(!inMTL.good())
    {
        cout << "ERROR OPENING MTL FILE" << endl;
        exit(1);
    }
    
    // Read MTL file
    while(!inMTL.eof())
    {
        string line;
        getline(inMTL, line);
        string type = line.substr(0,2);
        
        // Names
        if(type.compare("ne") == 0)
        {
            // Extract token
            string l = "newmtl ";
            materials[m] = line.substr(l.size());
            m++;
        }
        
        // Diffuses
        else if(type.compare("Kd") == 0)
        {
            // Copy line for parsing
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            // Extract tokens
            strtok(l, " ");
            for(int i=0; i<3; i++)
                diffuses[d][i] = atof(strtok(NULL, " "));
            
            // Wrap up
            delete[] l;
            d++;
        }
        
        // Speculars
        else if(type.compare("Ks") == 0)
        {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            for(int i=0; i<3; i++)
                speculars[s][i] = atof(strtok(NULL, " "));
            
            delete[] l;
            s++;
        }
    }
    
    // Close MTL file
    inMTL.close();
}

void writeCmaterials(string fp, string name, Model model, int firsts[], int counts[])
{
    // Append C file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Materials
    outC << "const int " << name << "Materials = " << model.materials << ";" << endl;
    outC << endl;
    
    // Firsts
    outC << "const int " << name << "Firsts[" << model.materials << "] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<model.materials; i++)
    {
        if(i == 0)
            firsts[i] = 0;
        else
            firsts[i] = firsts[i-1]+counts[i-1];
        
        outC << firsts[i] << "," << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    // Counts
    outC << "const int " << name << "Counts[" << model.materials << "] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<model.materials; i++)
    {
        outC << counts[i] << "," << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

void writeCdiffuses(string fp, string name, Model model, float diffuses[][3])
{
    // Append C file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Diffuses
    outC << "const float " << name << "Diffuses[" << model.materials << "][3] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<model.materials; i++)
    {
        outC << diffuses[i][0] << ", " << diffuses[i][1] << ", " << diffuses[i][2] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

void writeCspeculars(string fp, string name, Model model, float speculars[][3])
{
    // Append C file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Speculars
    outC << "const float " << name << "Speculars[" << model.materials << "][3] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<model.materials; i++)
    {
        outC << speculars[i][0] << ", " << speculars[i][1] << ", " << speculars[i][2] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

int main(int argc, const char * argv[])
{
    cout << "here";
    cout << "starting" << argv[1];
    // Files
    string nameOBJ = argv[1];
    string filepathOBJ = "source/" + nameOBJ + ".obj";
    string filepathMTL = "source/" + nameOBJ + ".mtl";
    string filepathH = "product/" + nameOBJ + ".h";
    string filepathC = "product/" + nameOBJ + ".c";
    
    // Model Info
    Model model = getOBJinfo(filepathOBJ);
    model.materials = getMTLinfo(filepathMTL);
    
    // Model Data
    float positions[model.positions][3];                // XYZ
    float texels[model.texels][2];                      // UV
    float normals[model.normals][3];                    // XYZ
    int faces[model.faces][10];                         // PTN PTN PTN M
    string* materials = new string[model.materials];    // Name
    float diffuses[model.materials][3];                 // RGB
    float speculars[model.materials][3];                // RGB
    extractMTLdata(filepathMTL, materials, diffuses, speculars);
    extractOBJdata(filepathOBJ, positions, texels, normals, faces, materials, model.materials);
    
    int firsts[model.materials];	// Starting vertex
    int counts[model.materials];	// Number of vertices
    
    // Write H file
    writeH(filepathH, nameOBJ, model);

    // Write C file
    writeCvertices(filepathC, nameOBJ, model);
    writeCpositions(filepathC, nameOBJ, model, faces, positions, counts);
    writeCtexels(filepathC, nameOBJ, model, faces, texels);
    writeCnormals(filepathC, nameOBJ, model, faces, normals);
    writeCmaterials(filepathC, nameOBJ, model, firsts, counts);
    writeCdiffuses(filepathC, nameOBJ, model, diffuses);
    writeCspeculars(filepathC, nameOBJ, model, speculars);
    
    return 0;
}

