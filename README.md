
# Projet PiloteVmc(// Développé par Marc Prieur (https://marco40github.wixsite.com/website/pilotevmc))  

    Ce projet est constitué des éléments suivant:  
        - Traitement (C++ esp8266)+page web dossier data.Base de départ: - Initial Github source : <https://github.com/hallard/LibTeleinfo>  
                                                                         - Modified Github source : <https://github.com/Doume/LibTeleinfo>  
                                                                         - Modified Github source : <https://github.com/marco402/LibTeleinfo/tree/rewrite-Wifinfo>  
        - Affichage (C++ arduino pro mini).
        - MessagesPiloteVmcVersSql(C++ QT).
        - TeleinfoVmc(html javascript php).Base de départ:https://github.com/BmdOnline/Teleinfo  

        Schémas designspark:  
            - Traitement.  
            - Affichage.  
            - Relais.  
            
        Sql  
            - Fichier pour générer la base de donnée.  
            
            A revoir:  
                - Le circuit TA12 situé dans le tableau de distribution capte trop de surtension: entrée A0 de l'ESP8366 HS.  
            A améliorer:  
                - Réglage de la luminositée de l'écran.
                - Remplacer la led jour nuit par une led multicolore.  
                - Alimentation problème de parasite /reset sur commutation du chauffage.  
                - Ajouter une carte SD pour mémoriser plus d'enregistrement.  
            Bug possible:  
                - Passage à minuit synchrone de la coupure wifi de la box programmée.  
                - Changement de mode par programme simultané d'un changement de mode manuel.  