#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>


struct Image{
    std::vector<std::vector<std::vector<int>>> pixels;
    size_t nbColonnes;
    size_t nbLignes;
    int maxColor;

    Image(const std::string& fichier){
        std::ifstream file(fichier);
        if (!file){
            throw std::string("Problème lors de l'ouverture du fichier");
            return;
        }
        std::string format;
        file >>format;
        if (format !="P3"){
            throw std::string("Pas le bon format ");
            return;
        }

        auto skip_comments = [&file]() { // Fonction pour skip les commentaires pour la lecture du fichier
            char c;
            while (file >> std::ws && file.peek() == '#') { //
                file.ignore(1024, '\n');
            }
        };

        skip_comments();
        file>> nbColonnes >> nbLignes ; // On récupère les caractéristiques de l'image
        skip_comments();
        file >>maxColor;
        std::cout << "Colonnes: " << nbColonnes << ", Lignes: " << nbLignes << ", maxColor: " << maxColor << std::endl;
        pixels.resize(nbLignes, std::vector<std::vector<int>>(nbColonnes, std::vector<int>(3)));
        // On lit les données de chaque pixels
        for (size_t i = 0; i < nbLignes; ++i) {
            for (size_t j = 0; j < nbColonnes; ++j) {
                file >> pixels[i][j][0] >> pixels[i][j][1] >> pixels[i][j][2]; // On lit dans l'ordre Rouge, Vert, Bleu
            }
        }

        file.close();
        std::cout << "Image chargée "<< std::endl;
    }

};

struct Hough_polaire{
    std::vector<std::vector<int>> compteur_droite;
    int rho_max;
    double step=0.1;

    Hough_polaire(const Image& image){
        int largeur=image.nbColonnes;
        int hauteur=image.nbLignes;
        rho_max=static_cast<int>(std::floor(std::sqrt(std::pow(largeur,2)+std::pow(hauteur,2))));
        int nb_step=2*rho_max/step;
        compteur_droite.resize(180,std::vector<int>(nb_step,0));
        for (size_t i=0;i<image.nbLignes;i++){
            for(size_t j=0;j<image.nbColonnes;j++){
                if (Bord(image.pixels[i][j])){
                    build_curve(i,j);
                }
            }
        }

    }
    bool Bord(const std::vector<int>& pixel){
        int intensite = (pixel[0]+pixel[1]+pixel[2])/3;
        return intensite<128;
    }

    void build_curve(size_t x, size_t y){
        for(int i=0;i<180;i++){
            double rho=x*std::cos(i*M_PI/180)+x*std::sin(i*M_PI/180);
            int rhoindex=static_cast<int>(rho + rho_max); // on remet l'indice dans [0,200]
            if (rhoindex>=0 && rhoindex<rho_max){
                compteur_droite[i][rhoindex]++;
            }
        }
    }
    std::pair<double,double> trouver_droite(){  // Fonction qui trouve la droite avec le plus de votes dans l'image de Hough
    int max_value=INT_MIN;
    std::pair<double,double> droite={0,0};
    double teta;
    double rho;
    for (int i=0;i<180;i++){
        for (int j=0;j<compteur_droite[0].size();j++){
            int valeur=compteur_droite[i][j];
            if (valeur>max_value){
                max_value=valeur;
                teta=i*M_PI/180;
                rho=-rho_max+j*step;
                droite={teta,rho};
            }
        }
    }
    return  droite;
    }

};
