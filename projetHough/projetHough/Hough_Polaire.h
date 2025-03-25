//
//  Hough_Polaire.h

//

#ifndef Hough_Polaire_h
#define Hough_Polaire_h

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include "Image.h"

struct Hough_polaire{
    std::vector<std::vector<int>> compteur_droite;
    int rho_max;
    double step=0.1;

    Hough_polaire(const Image& image){
        int largeur=image.nbColonnes;
        int hauteur=image.nbLignes;
        rho_max=static_cast<int>(std::floor(std::sqrt(std::pow(largeur,2)+std::pow(hauteur,2))));
        std::cout << "rhomax =" << rho_max<<std::endl;
        int nb_step=2*rho_max/step;
        compteur_droite.resize(180,std::vector<int>(nb_step,0));
        for (size_t i=0;i<image.nbLignes;i++){
            for(size_t j=0;j<image.nbColonnes;j++){
                if (Bord(image.pixels,j,i)){
                    build_curve(i,j);
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

    void build_curve(size_t x, size_t y){
        std:: cout << "x :" << x << ", y:" << y << std::endl;
        for(int i=0;i<180;i++){
            double rho=x*std::cos(i*M_PI/180)+y*std::sin(i*M_PI/180);
            int rhoindex=static_cast<int>((rho + rho_max)/step); // on remet l'indice dans [0,2rho_max]
            if (rhoindex >= 0 && rhoindex < compteur_droite[0].size()) {
                compteur_droite[i][rhoindex]++;
            }
        }
    }
    std::vector<std::pair<double,double>> trouver_droites(int seuil){  // Fonction qui trouve les droites avec le plus de votes dans l'image de Hough
        std::pair<double,double> droite={0,0};
        std::vector<std::pair<double,double>> vec_droites;
        double theta;
        double rho;
        for (int i=0;i<180;i++){
            for (int j=0;j<compteur_droite[0].size();j++){
                int valeur=compteur_droite[i][j];
                if (valeur>seuil){
                    theta=i*M_PI/180;
                    rho=-rho_max+j*step;
                    droite={theta,rho};
                    vec_droites.push_back(droite);
                }
            }
        }
        return vec_droites;
    }

};

std::vector<std::pair<double,double>> detect_droite_proche(std::vector<std::pair<double,double>> vec_droites,double epsilon_m,double epsilon_b){
    std::vector<std::pair<double,double>> res;
    bool check=false;
    for (auto droite1 : vec_droites){
        check=false;
        for (auto droite2 : res){
            if ((std::abs(droite1.first-droite2.first < epsilon_m)) && (std::abs(droite1.second-droite2.second))<epsilon_b){
                check=true;
                break;
            }
        }
        if (!check){
            res.push_back(droite1);
        }
    }
    return res;
}


#endif /* Hough_Polaire_h */
