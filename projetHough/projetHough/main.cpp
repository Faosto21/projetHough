#include "Hough.h"
#include "Hough_Polaire.h"
#include "Image.h"


int main(){
    
    // On commence par definir a quelle image on s interesse
    std::string chemin_image = "/Users/faosto/Desktop/imageM1.ppm";
    
    // Cas naif
    Image image_naif(chemin_image);
    Hough_naif hough_naif(image_naif);

    int seuil = 10;
    for (auto droite : regrouper_droites(hough_naif.trouver_droites(seuil), 0.5,5)) {
        double m = droite.first;
        double b = droite.second;
        std::cout << "Droite avec m = " << m << " et b = " << b << std::endl;
        image_naif.tracer_droite(m, b, {0,255,0});
    }
    image_naif.sauvegarder("/Users/faosto/Desktop/image_naif.ppm");

    
    // Cas polaire
    Image image_polaire(chemin_image);
    Hough_polaire hough_polaire(image_polaire);

    std::vector<std::pair<double,double>> vec_droites = hough_polaire.trouver_droites(seuil);

    for (auto droite : regrouper_droites(vec_droites, 0.5, 2.5)){
        double theta = droite.first;
        double rho = droite.second;
        std::cout << "Droite avec theta = " << theta << " et rho = " << rho << std::endl;
        image_polaire.tracer_droite_polaire(theta, rho, {0,255,0});
    }
    image_polaire.sauvegarder("/Users/faosto/Desktop/image_polaire.ppm");

    return 0;
}

