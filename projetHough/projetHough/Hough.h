#include <vector>
#include <iostream>
#include <fstream>


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

struct Hough{
    std::vector<std::vector<int>> compteur_droite;
    int mmin=-10;
    int mmax=10;
    double step=0.1;
    int nbstep=(mmax-mmin)/step;

    // On teste des valeurs de m allant de -10 à 10
    // b peut aller de -100 à 100
    Hough(const Image& image){
        compteur_droite.resize(nbstep,std::vector<int>(nbstep, 0));
        for(size_t i=0;i<image.nbLignes;i++){
            for(size_t j=0;j<image.nbColonnes;j++){
                if (Bord(image.pixels[i][j])){
                    build_lines(i,j);
                    }
            }
        }
    }

    bool Bord(const std::vector<int>& pixel){
        int intensite = (pixel[0]+pixel[1]+pixel[2])/3;
        return intensite<128;
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
    std::pair<int,int> trouver_droite(){  // Fonction qui trouve la droite avec le plus de vote dans l'image de Hough
    int max_value=INT_MIN;
    std::pair<int,int> droite={0,0};
    for (int i=0;i<compteur_droite.size();i++){
        for (int j=0;j<compteur_droite[0].size();j++){
            int valeur=compteur_droite[i][j];
            if (valeur>max_value){
                max_value=valeur;
                droite={mmin+i*step,j-compteur_droite.size() /2};
            }
        }
    }
    return  droite;
    }

};

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
                if (Bord(image.pixels[i][j])){
                    build_lines(i,j);
                    }
            }
        }
    }

    bool Bord(const std::vector<int>& pixel){
        int intensite = (pixel[0]+pixel[1]+pixel[2])/3;
        return intensite<128;
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
    std::pair<double,double> trouver_droite(){  // Fonction qui trouve la droite avec le plus de votes dans l'image de Hough
    int max_value=INT_MIN;
    std::pair<double,double> droite={0,0};
    for (int i=0;i<compteur_droite.size();i++){
        for (int j=0;j<compteur_droite[0].size();j++){
            int valeur=compteur_droite[i][j];
            if (valeur>max_value){
                max_value=valeur;
                droite={mmin+i*step,j-compteur_droite.size() /2};
            }
        }
    }
    return  droite;
    }

};
int main(){
    Image image("imageM1.ppm");
    Hough hough(image);
    std::cout<< hough.trouver_droite().first <<std::endl;

    return 0;
}
