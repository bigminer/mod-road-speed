package main

import (
	"flag"
	"log"

	"github.com/pthm/leeroy/azsdk/clientdata"
)

func main() {
	clientPath := flag.String("client", "", "path to a clean WoW 3.3.5a installation")
	outputPath := flag.String("out", "", "directory for Leeroy road coverage")
	flag.Parse()
	if *clientPath == "" || *outputPath == "" {
		log.Fatal("provide -client and -out")
	}

	client, err := clientdata.OpenClient(*clientPath)
	if err != nil {
		log.Fatal(err)
	}
	count, err := client.Roads(1, *outputPath)
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("Kalimdor road tiles: %d", count)
}
