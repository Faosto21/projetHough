//
//  main.cpp
//  projetHough
//
//  Created by Faosto Blanquet on 19/03/2025.
//

int main() {
    cv::Mat imageCV = cv::imread("/Users/faosto/Desktop/imageM1.ppm", cv::IMREAD_GRAYSCALE);
    
    cv::Mat edges;
    cv::Canny(imageCV, edges, 50, 150);
    cv::imshow("Canny edge", edges);

    Image imageHough("/Users/faosto/Desktop/imageM1.ppm");
    Hough_polaire hough(imageHough);
    std::cout<< hough.trouver_droite().first <<std::endl;
    std::cout<< hough.trouver_droite().second <<std::endl;

    
    // Chemin de l'image
    std::string imagePath = "/Users/faosto/Desktop/imageM1.ppm";

    // Chargement de l'image avec OpenCV
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cerr << "Erreur : impossible de charger l'image." << std::endl;
        return 1;
    }

    // Debug : dimensions de l'image
    std::cout << "Image chargée : " << image.cols << " x " << image.rows << std::endl;

    // Exemple de droite : remplace m et b avec les valeurs que tu obtiens via Hough
    double teta = hough.trouver_droite().first; // coef directeur
    double rho = hough.trouver_droite().second; // ordonnee à l’origine
    
//    double m = -cos(teta) / sin(teta);
//    double b = rho / sin(teta);

    double m = hough.trouver_droite().first;
    double b = hough.trouver_droite().second;
    
    
    // Definition des deux extremites de la droite
    int x1 = 0;
    int y1 = static_cast<int>(m * x1 + b);
    int x2 = image.cols;
    int y2 = static_cast<int>(m * x2 + b);

    // Dessin de la droite en noir
    cv::line(image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), 2);

    // Affichage du resultat
    cv::imshow("Ligne détectée", image);
    //cv::imwrite("/Users/faosto/Desktop/resultat.png", image); // sauvegarde

    cv::waitKey(0);
    return 0;
}
