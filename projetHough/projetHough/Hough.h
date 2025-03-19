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
                file >> pixels[i][j][0] >> pixels[i][j][1] >> pixels[i][j][2]; // On lie dans l'ordre Rouge, Vert, Bleu
            }
        }

        file.close();
        std::cout << "Image chargée "<< std::endl;
    }

};

struct Hough{
    std::vector<std::vector<int>> points;

    Hough(const Image& image){
      points.resize(image.nbLignes,std::vector<int>(image.nbColonnes, 0));
    }
};


int main(){
    Image image("imageM1.ppm");
    return 0;
}
