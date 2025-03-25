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
    std::vector<std::vector<int>> compteur_droite; // compteur_droite[i][j] compte le nombre de sinusoïdales qui passent par le point (i,j)
    int rho_max;
    double step=0.1;

    Hough_polaire(const Image& image){
        int largeur=image.nbColonnes;
        int hauteur=image.nbLignes;
        rho_max=static_cast<int>(std::floor(std::sqrt(std::pow(largeur,2)+std::pow(hauteur,2)))); // Calcul du rho_max en fonction des dimensions de l'image
        std::cout << "rhomax =" << rho_max<<std::endl;
        int nb_step=2*rho_max/step;
        compteur_droite.resize(180,std::vector<int>(nb_step,0));
        for (size_t i=0;i<image.nbLignes;i++){
            for(size_t j=0;j<image.nbColonnes;j++){
                if (Bord(image.pixels,j,i)){
                    build_curve(i,j);  // On 'dessine' les courbes uniquement pour les points considérés comme au bord
                }
            }
        }

    }
    bool Bord(const std::vector<std::vector<std::vector<int>>>& pixels, size_t x, size_t y) {
        // Fonction qui détermine si un point est au bord d'une droite à partir du gradient de couleur du point
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
        // Fonction qui 'dessine' la sinusoïdales associé au point (x,y) dans l'espace de Hough en ajoutant 1 à tout les points traversés par cette sinusoïdale
        std:: cout << "x :" << x << ", y:" << y << std::endl;
        for(int i=0;i<180;i++){
            double rho=x*std::cos(i*M_PI/180)+y*std::sin(i*M_PI/180);
            int rhoindex=static_cast<int>((rho + rho_max)/step); // on remet l'indice dans [0,2rho_max]
            if (rhoindex >= 0 && rhoindex < compteur_droite[0].size()) {
                compteur_droite[i][rhoindex]++;
            }
        }
    }
    std::vector<std::pair<double,double>> trouver_droites(int seuil){ 
        // Fonction qui trouve les points avec un nombre de votes au dessus d'un certain seuil dans l'espace de Hough, et donc les droites dans l'image initiale
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



#endif /* Hough_Polaire_h */
