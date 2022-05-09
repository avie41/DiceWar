#pragma once
// v 1.0

#include "IInfoLib.h"

// Structure définissant les caractéristiques d'une cellule
struct SCellInfo
{
	unsigned int id;	// Id de la cellule
	unsigned int owner;	// Id du joueur qui la possède
	unsigned int nbDices;	// Nombre de dés
};

struct SGameState
{
	SCellInfo *cells;	// Tableau d'informations sur le contenu des cellules
	unsigned int nbCells;
	unsigned int points[8];	// Points de chaque joueur
	unsigned int diceStock[8];	// Réserve de dés de chaque joueur
};

// Structure définissant les caractéristiques d'une cellule
struct SCell;
using pSCell = SCell*;
struct SCell
{
	SCellInfo infos;	// Informations sur la cellule
	pSCell *neighbors;	// Tableau de pointeurs vers des cellules voisines
	unsigned int nbNeighbors;	// Nombre de cellules voisines
};

// Structure définissant la carte globale du jeu
struct SMap
{
	SCell *cells;	// Tableau des cellules
	unsigned int nbCells;	// Nombre de cellules
};

// Structure définissant les paramètres d'un coup joué
struct STurn
{
	unsigned int cellFrom;
	unsigned int cellTo;
};

struct IStrategy
{
	virtual ~IStrategy() {}

	// Fonction à appeler au début de chaque partie
	//	* Ne pas oublier pour l'arbitre de dupliquer toute la structure map pour chaque appel !
	virtual void InitGame(unsigned int id, unsigned int nbPlayer, const SMap* map) = 0;

	// Fonction à appeler à chaque tour sur la stratégie et tant que le retour de fonction est vrai et qu'il n'y a pas d'erreur.
	//	* En cas d'erreur, rétablir la carte dans l'état initial avant le premier tour du joueur.
	// valeur de retour : booléen - false : coups terminés, true : structure turn complétée avec un nouveau coup à jouer.
	// gameTurn : tour de jeu (permet à la stratégie de savoir si elle a échouée au tour précédant)
	virtual bool PlayTurn(unsigned int gameTurn, const SGameState* state, STurn* turn) = 0;

	// Fonction à appeler à la fin de chaque partie
	virtual void EndGame(unsigned int idWinner) = 0;
};

// Types prédéfinis pour simplifier la récupération des fonctions dans la librairie
using pStrategyFactory = IStrategy * (*)();
using pStrategyDelete = void (*)(IStrategy* obj);
