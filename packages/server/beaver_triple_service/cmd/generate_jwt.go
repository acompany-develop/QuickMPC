package cmd

import (
	"bufio"
	"bytes"
	"crypto/rand"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"path"
	"strings"

	"github.com/golang-jwt/jwt/v4"
	"github.com/google/uuid"
	"github.com/spf13/cobra"
	"gopkg.in/yaml.v3"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
)

type Options struct {
    file_path  string
    output_dir string
}

var (
    o = &Options{}
)

func store_client_env(path string, token string) error {
	fp, err := os.OpenFile(path, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0600)
	defer fp.Close()
	if err != nil {
		return err
	}

	writer := bufio.NewWriter(fp)
	defer writer.Flush()

	fmt.Fprintf(writer, "BTS_TOKEN=%s\n", token)

	return nil
}

func store_server_env(path string, secrets string, claim jwt_types.Claim) error {
	fp, err := os.OpenFile(path, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0600)
	defer fp.Close()
	if err != nil {
		return err
	}

	writer := bufio.NewWriter(fp)
	defer writer.Flush()

	fmt.Fprintf(writer, "JWT_SECRET_KEY=%s\n", secrets)

	return nil
}

var generateJwtCmd = &cobra.Command{
	Use:   "generateJwt",
	Short: "Generate jwt token",
	Run: func(cmd *cobra.Command, args []string) {
		raw, err := os.ReadFile(o.file_path)
		if err != nil {
			log.Fatalln(err)
		}

		// convert yaml to json
		dict := make(map[string]interface{})
		err = yaml.Unmarshal(raw, &dict)
		if err != nil {
			log.Fatalln(err)
		}

		raw, err = json.Marshal(dict)
		if err != nil {
			log.Fatalln(err)
		}

		var claim jwt_types.Claim
		err = json.Unmarshal(raw, &claim)
		if err != nil {
			log.Fatalln(err)
		}

		// generate, and set
		room_uuid, err := uuid.NewRandom()
		if err != nil {
			log.Fatalln(err)
		}
		claim.RoomUUID = room_uuid.String()

		subject, err := uuid.NewRandom()
		if err != nil {
			log.Fatalln(err)
		}
		claim.Subject = subject.String()

		// show generated json
		raw, err = json.Marshal(claim)
		if err != nil {
			log.Fatalln(err)
		}

		var buf bytes.Buffer
		err = json.Indent(&buf, raw, "", "  ")
		if err != nil {
			log.Fatalln(err)
		}
		log.Printf("json: %s\n", buf.String())

		// build token
		const RANDOM_BITS = 256
		const RANDOM_BYTES = RANDOM_BITS / 8
		secrets := make([]byte, RANDOM_BYTES)
		_, err = rand.Read(secrets)
		if err != nil {
			log.Fatalln(err)
		}

		token := jwt.NewWithClaims(jwt.SigningMethodHS256, claim)
		signed_token_str, err := token.SignedString(secrets)
		if err != nil {
			log.Fatalln(err)
		}

		// show token
		encoded_secrets := base64.StdEncoding.EncodeToString(secrets)
		log.Printf("token: %s\n", signed_token_str)
		log.Printf("key base64 encoded: %s\n", encoded_secrets)

		// save part of `.env` files for client and server
		err = os.MkdirAll(
			o.output_dir, 0700)
		if err != nil {
			log.Fatalln(err)
		}

		base := path.Base(o.file_path)
		ext := path.Ext(base)
		filename := base[:len(base)-len(ext)]

		err = store_client_env(
			path.Join(
				o.output_dir,
				strings.Join([]string{"client", filename, "env"}, ".")),
			signed_token_str)
		if err != nil {
			log.Fatalln(err)
		}

		err = store_server_env(
			path.Join(
				o.output_dir,
				strings.Join([]string{"server", filename, "env"}, ".")),
			encoded_secrets, claim)
		if err != nil {
			log.Fatalln(err)
		}
	},
}

func init() {
	rootCmd.AddCommand(generateJwtCmd)
	work_dir, err := os.Getwd()
	if err != nil {
		log.Printf("WARN: os.Getwd() is failed, err = %s\n", err)
	}
	default_inout_root := path.Join(work_dir, "cmd", "jwt_generator", "sample")
	generateJwtCmd.Flags().StringVarP(&o.file_path, "file", "f", path.Join(default_inout_root, "sample.yml"), "configuration file path")
	generateJwtCmd.Flags().StringVarP(&o.output_dir, "output", "o", default_inout_root, "directory which stores output files")
}
