#ADAS-CPP

#Lancement du programme en mode embarqué :
C'est le mode qui se lance par défaut:
./ShapeColorDetector -counts

#Lancement du programme en mode Debug (avec chemin vers le fichier text contenant les chemin vers les images à traiter)
./ShapeColorDetector -list /filePath/text.txt

#Lancement du programme en mode Développement
./ShapeColorDetector -debug

#Options : (à ajouter à la fin de la commande)
--noPipe : Sans utilisation des pipes (pour lancer le programme sans lancer les flags)
--noSave : Ne sauvegarde pas les images
--show : Utilisation que en mode Debug. Pause entre chaque frame et affichage.

#Création du fichier text.txt contenant les chemins vers les images à traiter :
list.sh est dans /scripts
./list.sh /home/pi/Documents/Global/ jpg > /home/pi/Documents/ADAS-CPP/text.txt