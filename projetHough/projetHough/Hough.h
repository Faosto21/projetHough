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
      std::vector<int> ligne(image.nbColonnes);
      std::vector<std::vector<int>> vec(image.nbLignes,ligne);
      points=vec;

};


int main(){
    return 0;
}
