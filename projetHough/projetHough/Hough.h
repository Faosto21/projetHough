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
                pixels[j][i][0] = r;
                pixels[j][i][1] = g;
                pixels[j][i][2] = b;
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
        rho_max=static_cast<int>(std::floor(std::sqrt(std::pow(largeur,2)+std::pow(hauteur,2))))/2;
        std::cout << "rhomax =" << rho_max<<std::endl;
        int nb_step=2*rho_max/step;
        compteur_droite.resize(180,std::vector<int>(nb_step,0));
        for (size_t i=0;i<image.nbLignes;i++){
            for(size_t j=0;j<image.nbColonnes;j++){
                if (Bord(image.pixels,i,j)){
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
        return gradient > 60;
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

std::vector<std::pair<double, double>> regrouper_droites(std::vector<std::pair<double, double>> vec_droites, double epsilon_theta, double epsilon_rho)
{
    std::vector<std::pair<double, double>> groupes;  //Fonction qui regroupe les droites en groupe de proximité
    std::vector<bool> utilisé(vec_droites.size(), false); // Et ensuite prends la moyenne du teta et du rho pour choisir un représentant du groupe

    for (size_t i = 0; i < vec_droites.size(); i++) {
        if (utilisé[i]) continue;

        double somme_theta = vec_droites[i].first;
        double somme_rho = vec_droites[i].second;
        int count = 1;
        utilisé[i] = true;

        for (size_t j = i + 1; j < vec_droites.size(); j++) {
            if (!utilisé[j] &&
                std::abs(vec_droites[i].first - vec_droites[j].first) < epsilon_theta &&
                std::abs(vec_droites[i].second - vec_droites[j].second) < epsilon_rho)
            {
                somme_theta += vec_droites[j].first;
                somme_rho += vec_droites[j].second;
                count++;
                utilisé[j] = true;
            }
        }

        groupes.push_back({somme_theta / count, somme_rho / count});
    }

    return groupes;
}


int main(){
    Image image("imageM1.ppm");
    Hough_polaire hough(image);
    int seuil =7;
    std::vector<std::pair<double,double>> vec_droites=hough.trouver_droites(seuil);
    std::cout << "Longueur initiale =" << vec_droites.size() << std::endl;
    std::cout << "En regroupant les droites proches =" << regrouper_droites(vec_droites,0.5,2.5).size() << std::endl;
    for (auto droite :regrouper_droites(vec_droites,0.5,2.5)){
        double teta=droite.first;
        double rho= droite.second;
        std::cout << "Droite avec teta =" << teta << "et rho =" << rho << std::endl;
        if (teta==0){
            std::cout << " Soit x=" << rho << std::endl;
        }
        else {
            std::cout << "y = "<< -std::cos(teta)/std::sin(teta) << "x + "<< rho/std::sin(teta) << std::endl;
        }
    }


    return 0;
}
