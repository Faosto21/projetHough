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
        skip_comments();
        std::cout << "Colonnes: " << nbColonnes << ", Lignes: " << nbLignes << ", maxColor: " << maxColor << std::endl;
        pixels.resize(nbLignes, std::vector<std::vector<int>>(nbColonnes, std::vector<int>(3)));
        // On lit les données de chaque pixels
        for (size_t i = 0; i < nbLignes; ++i) {
            for (size_t j = 0; j < nbColonnes; ++j) {
                int r, g, b;
                file >> r >> g >> b;
                pixels[i][j][0] = r;
                pixels[i][j][1] = g;
                pixels[i][j][2] = b;
            }
        }

        file.close();
        std::cout << "Image chargée "<< std::endl;
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
                    std::cout << "i=" << i << " et j =" << j<<std::endl;
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
    std::vector<std::pair<double,double>> trouver_droites(int seuil){  // Fonction qui trouve la droite avec le plus de votes dans l'image de Hough
        std::pair<double,double> droite={0,0};
        std::vector<std::pair<double,double>> vec_droites;
        double teta;
        double rho;
        for (int i=0;i<180;i++){
            for (int j=0;j<compteur_droite[0].size();j++){
                int valeur=compteur_droite[i][j];
                if (valeur>seuil){
                    teta=i*M_PI/180;
                    rho=-rho_max+j*step;
                    droite={teta,rho};
                    vec_droites.push_back(droite);
                }
            }
        }
        return vec_droites;
    }

};


int main(){
    Image image("imageM1.ppm");
    Hough_naif hough(image);
    int seuil =10;
    std::vector<std::pair<double,double>> vec_droites=hough.trouver_droites(seuil);
    for (auto droite :vec_droites){
        std::cout << "Droite avec m =" << droite.first<< "et b =" << droite.second << std::endl;
    }


    return 0;
}

