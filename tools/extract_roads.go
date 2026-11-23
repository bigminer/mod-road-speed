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
	for _, mapID := range []int{0, 1, 530, 571} {
		count, err := client.Roads(mapID, *outputPath)
		if err != nil {
			log.Fatalf("map %d: %v", mapID, err)
		}
		log.Printf("map %d: %d road tiles", mapID, count)
	}
}
