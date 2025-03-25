//
//  Image.h
//

#ifndef Image_h
#define Image_h

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

std::vector<std::pair<double, double>> regrouper_droites(std::vector<std::pair<double, double>> vec_droites, double epsilon_theta, double epsilon_rho)
{
    std::vector<std::pair<double, double>> groupes;  //Fonction qui regroupe les droites en groupe de proximité
    std::vector<bool> utilisé(vec_droites.size(), false); // Et ensuite prends la moyenne du theta et du rho pour choisir un représentant du groupe

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
    
    // Methode pour tracer les droites manuellement dans le cas naif
    
    void tracer_droite(double m, double b, std::vector<int> couleur = {0, 0, 0}) {
        if (std::abs(m) <= 1){
            for (size_t x = 0; x < nbColonnes; ++x) {
                double y = m * x + b;
                int int_y = static_cast<int>(round(y)); // on arrondit y et on le transforme en entier
                if (int_y >= 0 && int_y < nbLignes) {
                    pixels[int_y][x] = couleur;
                }
            }
        }
        else{
            for (size_t y = 0; y < nbLignes; ++y) {
                double x = (y - b)/m;
                int int_x = static_cast<int>(round(x)); // on arrondit x et on le transforme en entier
                if (int_x >= 0 && y < nbColonnes) {
                    pixels[y][int_x] = couleur;
                }
            }
        }
        
    }
    
    // Methode pour tracer les droites manuellement dans le cas polaire
    void tracer_droite_polaire(double theta, double rho, std::vector<int> couleur = {0, 0, 0}) {
        if (sin(theta) <= 1e-3){
            int x = static_cast<int>(round(rho));
            for (size_t y = 0; y < nbLignes; ++y) {
                pixels[y][x] = couleur;
            }
        }
        else{
            double m = -cos(theta) / sin(theta);
            double b = rho / sin(theta);
            if (std::abs(m) <= 1){
                for (size_t x = 0; x < nbColonnes; ++x) {
                    double y = m * x + b;
                    int int_y = static_cast<int>(round(y)); // on arrondit y et on le transforme en entier
                    if (int_y >= 0 && int_y < nbLignes) {
                        pixels[int_y][x] = couleur;
                    }
                }
            }
            else{
                for (size_t y = 0; y < nbLignes; ++y) {
                    double x = (y - b)/m;
                    int int_x = static_cast<int>(round(x)); // on arrondit x et on le transforme en entier
                    if (int_x >= 0 && int_x < nbColonnes) {
                        pixels[y][int_x] = couleur;
                    }
                }
            }
        }
        
    }
    
    


    // Methode pour sauvegarder l'image
    
    void sauvegarder(const std::string& nomFichier) const {
        std::ofstream file(nomFichier);
        if (!file) {
            throw std::string("Impossible d’ouvrir le fichier");
        }

        file << "P3\n";
        file << nbColonnes << " " << nbLignes << "\n";
        file << maxColor << "\n";

        for (size_t i = 0; i < nbLignes; ++i) {
            for (size_t j = 0; j < nbColonnes; ++j) {
                for (int c = 0; c < 3; ++c) {
                    file << pixels[i][j][c] << " ";
                }
            }
            file << "\n";
        }
        file.close();
    }


};
#endif /* Image_h */

