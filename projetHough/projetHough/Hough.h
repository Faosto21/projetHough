#include <vector>
#include <iostream>
#include <fstream>

struct Image{
    std::vector<std::vector<std::vector<int>>> pixels;
    int nbColonnes;
    int nbLignes;
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
        file>> nbColonnes >> nbLignes >> maxColor; // On récupère les caractéristiques de l'image
        pixels.resize(nbLignes, std::vector<std::vector<int>>(nbColonnes, std::vector<int>(3)));
        // On lit les données de chaque pixels
        for (int i = 0; i < nbLignes; ++i) {
            for (int j = 0; j < nbColonnes; ++j) {
                file >> pixels[i][j][0] >> pixels[i][j][1] >> pixels[i][j][2]; // On lie dans l'ordre Rouge, Vert, Bleu
            }
        }

        file.close();
        std::cout << "Image chargée "<< std::endl;
    }

};

struct Hough{
    std::vector<std::vector<int>> points;

    Hough(Image image):{
      std::vector<int> ligne(image.nbColonnes);
      std::vector<std::vector<int>> vec(image.nbLignes,ligne);
      points=vec;

};


int main(){
    return 0;
}
