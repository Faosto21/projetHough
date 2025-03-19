#include <vector>

struct Image{
    std::vector<std::vector<int>> pixels;
    int nbColonnes;
    int nbLignes;

    Image():pixels(0),nbColonnes(0),nbLignes(0){
    }
    Image(std::vector<std::vector<int>> tableau): pixels(tableau),nbColonnes(tableau[0].size()),nbLignes(tableau.size()){}

};

struct Hough{
    std::vector<std::vector<int>> points;

    Hough(Image image):{
      std::vector<std::vector<int>> vec(image.nbLignes,std::vector<int> ligne(image.nbColonnes)

};


int main(){
    return 0;
}
