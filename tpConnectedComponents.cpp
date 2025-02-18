#include "tpConnectedComponents.h"
#include <cmath>
#include <algorithm>
#include <tuple>
#include <vector>
#include <stack>
#include <map>
using namespace cv;
using namespace std;


/**
    Performs a labeling of image connected component with 4 connectivity
    with a depth-first exploration.
    Any non zero pixel of the image is considered as present.
*/
cv::Mat ccLabel(cv::Mat image)
{
    // Création d'une matrice de même taille que l'image d'entrée
    Mat res = Mat::zeros(image.rows, image.cols, CV_32SC1);

    Mat visited = Mat::zeros(image.rows, image.cols, CV_8UC1); 

    int label = 1; 
    int dx[4] = {-1, 1, 0, 0};  // Déplacement horizontal
    int dy[4] = {0, 0, -1, 1};  // Déplacement vertical

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            if (image.at<uchar>(i, j) == 1 && !visited.at<uchar>(i, j))
            {
                // Création d'une pile pour DFS
                stack<Point> pile;
                pile.push(Point(j, i));
                visited.at<uchar>(i, j) = 1;  // Marque comme visité
                res.at<int>(i, j) = label; 

                //  Parcours des pixels connectés (DFS)
                while (!pile.empty())
                {
                    Point p = pile.top(); 
                    pile.pop(); 

                    for (int k = 0; k < 4; k++)
                    {
                        int nx = p.x + dx[k];
                        int ny = p.y + dy[k]; 

                        if (nx >= 0 && nx < image.cols && ny >= 0 && ny < image.rows)
                        {
                            if (image.at<uchar>(ny, nx) == 1 && !visited.at<uchar>(ny, nx))
                            {
                                visited.at<uchar>(ny, nx) = 1;  
                                res.at<int>(ny, nx) = label; 
                                pile.push(Point(nx, ny));  
                            }
                        }
                    }
                }
                label++;
            }
        }
    }

    return res; 
}


/**
    Deletes the connected components (4 connectivity) containg less than size pixels.
*/
cv::Mat ccAreaFilter(cv::Mat image, int size)
{
    assert(size > 0);

    Mat labeledImage = ccLabel(image);

    std::map<int, int> labelSizes;
    for (int i = 0; i < labeledImage.rows; i++)
    {
        for (int j = 0; j < labeledImage.cols; j++)
        {
            int label = labeledImage.at<int>(i, j);
            if (label > 0)
            {
                labelSizes[label]++;
            }
        }
    }

    Mat res = Mat::zeros(image.rows, image.cols, CV_8UC1);

    for (int i = 0; i < labeledImage.rows; i++)
    {
        for (int j = 0; j < labeledImage.cols; j++)
        {
            int label = labeledImage.at<int>(i, j);
            if (label > 0 && labelSizes[label] >= size)
            {
                res.at<uchar>(i, j) = 255;  
            }
        }
    }

    return res;
}



/**
    Performs a labeling of image connected component with 4 connectivity using a
    2 pass algorithm.
    Any non zero pixel of the image is considered as present.
*/
cv::Mat ccTwoPassLabel(cv::Mat image)
{
    Mat res = Mat::zeros(image.rows, image.cols, CV_32SC1); // Matrice des labels
    int nextLabel = 1; 
    std::map<int, int> label_equivalence; 

    int dx[2] = {-1, 0}; // Gauche
    int dy[2] = {0, -1}; // Haut


    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            if (image.at<uchar>(i, j) == 1) 
            {
                cout << "Pixel détecté à (" << i << ", " << j << ") -> Ajout d’un label.\n"; //debug
                
                std::vector<int> neighbors; // Liste des labels voisins

                for (int k = 0; k < 2; k++)
                {
                    int nx = j + dx[k];
                    int ny = i + dy[k];

                    if (nx >= 0 && ny >= 0) // Vérifie si le voisin est valide
                    {
                        int neighbor_label = res.at<int>(ny, nx);
                        if (neighbor_label > 0)
                        {
                            neighbors.push_back(neighbor_label);
                        }
                    }
                }

                if (neighbors.empty()) // Aucun voisin → Nouveau label
                {
                    res.at<int>(i, j) = nextLabel;
                    label_equivalence[nextLabel] = nextLabel; 
                    nextLabel++; 
                }
                else // Un ou plusieurs voisins existants
                {
                    int minLabel = *std::min_element(neighbors.begin(), neighbors.end());
                    res.at<int>(i, j) = minLabel;

                    for (int label : neighbors)
                    {
                        label_equivalence[label] = std::min(label_equivalence[label], minLabel);
                    }
                }
            }
        }
    }

    // 2ᵉ Passe 
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            int current_label = res.at<int>(i, j);
            if (current_label > 0){
                while (label_equivalence[current_label] != current_label)
                {
                    current_label = label_equivalence[current_label];
                }
                res.at<int>(i, j) = current_label;
            }
        }
    }

    return res;
}
/**
 * @brief Détecte et remplit les trous dans une image segmentée.
 *        Utilise BFS pour identifier les trous.
 *        (BONUS)
 */
cv::Mat fillHoles(Mat labeledImage)
{
    Mat filled = labeledImage.clone();
    int rows = labeledImage.rows;
    int cols = labeledImage.cols;
    Mat visited = Mat::zeros(rows, cols, CV_8UC1);

    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};

    // File 
    queue<Point> q;

    // Détection des pixels de bord (marquage des régions extérieures)
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if ((i == 0 || j == 0 || i == rows - 1 || j == cols - 1) && labeledImage.at<int>(i, j) == 0)
            {
                q.push(Point(j, i));
                visited.at<uchar>(i, j) = 1; // Marquer comme extérieur
            }
        }
    }

    // identifier les trous
    while (!q.empty())
    {
        Point p = q.front();
        q.pop();

        for (int k = 0; k < 4; k++)
        {
            int nx = p.x + dx[k];
            int ny = p.y + dy[k];

            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows && visited.at<uchar>(ny, nx) == 0 && labeledImage.at<int>(ny, nx) == 0)
            {
                visited.at<uchar>(ny, nx) = 1;
                q.push(Point(nx, ny));
            }
        }
    }

    // Vérification des trous détectés
cout << " Vérification des trous détectés :\n";
bool trousDetectes = false;
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        if (visited.at<uchar>(i, j) == 0 && labeledImage.at<int>(i, j) == 0) {
            cout << "Trou détecté à (" << i << ", " << j << ")\n";
            trousDetectes = true;
        }
    }
}
if (!trousDetectes) {
    cout << "Aucun trou détecté.\n";
}

    cout << " Vérification des trous détectés :\n";
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (visited.at<uchar>(i, j) == 0 && labeledImage.at<int>(i, j) == 0)
            {
                cout << "Trou détecté à (" << i << ", " << j << ")\n";
            }
        }
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (labeledImage.at<int>(i, j) == 0 && visited.at<uchar>(i, j) == 0)
            {
                filled.at<int>(i, j) = -1; 
            }
        }
    }

    return filled;
}


/**
 * @brief Construit un arbre d’adjacence basé sur les labels des composantes connectées.
 */
std::map<int, std::set<int>> buildAdjacencyGraph(cv::Mat labeledImage)
{
    std::map<int, std::set<int>> adjacencyGraph;
    int rows = labeledImage.rows;
    int cols = labeledImage.cols;

    int dx[4] = {-1, 1, 0, 0};  
    int dy[4] = {0, 0, -1, 1};

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int label = labeledImage.at<int>(i, j);
            if (label > 0)  
            {
                for (int k = 0; k < 4; k++)
                {
                    int nx = j + dx[k];
                    int ny = i + dy[k];

                    if (nx >= 0 && nx < cols && ny >= 0 && ny < rows)
                    {
                        int neighbor_label = labeledImage.at<int>(ny, nx);
                        if (neighbor_label > 0 && neighbor_label != label)
                        {
                            adjacencyGraph[label].insert(neighbor_label);
                            adjacencyGraph[neighbor_label].insert(label);
                        }
                    }
                }
            }
        }
    }

    cout << "Construction du graphe d'adjacence terminée !\n";
    cout << "Relations détectées dans le graphe d’adjacence :\n";
    for (const auto &pair : adjacencyGraph)
    {
        cout << "Composante " << pair.first << " est connectée à : ";
        for (int neighbor : pair.second)
        {
            cout << neighbor << " ";
        }
        cout << endl;
    }

    return adjacencyGraph;
}
