//
//  Hough.h
//

#ifndef Hough_h
#define Hough_h

#include <vector>
#include <iostream>
#include <fstream>
#include "Image.h"


struct Hough_naif{
    std::vector<std::vector<int>> compteur_droite;
    int mmin=-10;
    int mmax=10;
    double step=0.1;
    int nbstep=(mmax-mmin)/step;

    // On teste des valeurs de m allant de -10 à 10
    // b peut aller de -100 à 100
    Hough_naif(const Image& image){
        compteur_droite.resize(nbstep,std::vector<int>(nbstep, 0));
        for(size_t i=0;i<image.nbLignes;i++){
            for(size_t j=0;j<image.nbColonnes;j++){
                if (Bord(image.pixels,i,j)){
                    std::cout << "i=" << i << " et j =" << j<<std::endl;
                    build_lines(j,i);
                    }
            }
        }
    }

    bool Bord(const std::vector<std::vector<std::vector<int>>>& pixels, size_t x, size_t y) {
        int hauteur = pixels.size();
        int largeur = pixels[0].size();

        auto intensity = [&](int i, int j) { //Fonction qui calcule l'intensité de chaque pixel
            return (pixels[i][j][0] + pixels[i][j][1] + pixels[i][j][2]) / 3;
        };

        // On vérifie si on est pas sur les bords
        if (x <= 0 || x >= hauteur - 1 || y <= 0 || y >= largeur - 1) {
            return false;
        }

        // On calcule le gradient dans chaque direction
        int gx = intensity(x + 1, y) - intensity(x - 1, y);
        int gy = intensity(x, y + 1) - intensity(x, y - 1);

        int gradient = std::sqrt(gx * gx + gy * gy);

        //
        return gradient > 30;
    }
    
    void build_lines(size_t x, size_t y){
        for(int i=0;i<nbstep;i++){
            double m=mmin+i*step;
            double b=y-m*x;
            int mindex=i;
            int bindex=static_cast<int>(b + compteur_droite.size() / 2); // on remet l'indice dans [0,200]
            if (bindex>=0 && bindex<compteur_droite.size()){
                compteur_droite[mindex][bindex]++;
            }
        }
    }
    std::vector<std::pair<double,double>> trouver_droites(int seuil){  // Fonction qui trouve la droite avec le plus de votes dans l'image de Hough
        std::pair<double,double> droite={0,0};
        std::vector<std::pair<double,double>> vec_droites;
        double m;
        double b;
        for (int i=0;i<compteur_droite.size();i++){
            for (int j=0;j<compteur_droite[0].size();j++){
                int valeur=compteur_droite[i][j];
                if (valeur>seuil){
                    m=mmin+i*step;
                    b=j-compteur_droite.size()/2;
                    droite={m,b};
                    vec_droites.push_back(droite);
                }
            }
        }
        return vec_droites;
    }

};
#endif /* Hough_h */
