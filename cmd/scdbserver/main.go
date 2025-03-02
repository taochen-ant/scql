// Copyright 2023 Ant Group Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package main

import (
	"flag"
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/gin-gonic/gin"
	log "github.com/sirupsen/logrus"

	"github.com/secretflow/scql/pkg/executor"
	"github.com/secretflow/scql/pkg/grm"
	"github.com/secretflow/scql/pkg/grm/stdgrm"
	"github.com/secretflow/scql/pkg/grm/toygrm"
	"github.com/secretflow/scql/pkg/scdb/server"
	"github.com/secretflow/scql/pkg/scdb/storage"

	"gopkg.in/natefinch/lumberjack.v2"
)

const (
	defaultConfigPath = "cmd/scdbserver/config.yml"
)

// custom monitor formatter, e.g.: "2020-07-14 16:59:47.7144 INFO main.go:107 |msg"
type CustomMonitorFormatter struct {
	log.TextFormatter
}

func (f *CustomMonitorFormatter) Format(entry *log.Entry) ([]byte, error) {
	var fileWithLine string
	if entry.HasCaller() {
		fileWithLine = fmt.Sprintf("%s:%d", filepath.Base(entry.Caller.File), entry.Caller.Line)
	} else {
		fileWithLine = ":"
	}
	return []byte(fmt.Sprintf("%s %s %s %s\n", entry.Time.Format(f.TimestampFormat),
		strings.ToUpper(entry.Level.String()), fileWithLine, entry.Message)), nil
}

const (
	LogFileName                 = "logs/scdbserver.log"
	LogOptionMaxSizeInMegaBytes = 500
	LogOptionMaxBackupsCount    = 10
	LogOptionMaxAgeInDays       = 0
	LogOptionCompress           = false
)

func main() {
	log.SetReportCaller(true)
	log.SetFormatter(&CustomMonitorFormatter{log.TextFormatter{TimestampFormat: "2006-01-02 15:04:05.123"}})
	rollingLogger := &lumberjack.Logger{
		Filename:   LogFileName,
		MaxSize:    LogOptionMaxSizeInMegaBytes, // megabytes
		MaxBackups: LogOptionMaxBackupsCount,
		MaxAge:     LogOptionMaxAgeInDays, //days
		Compress:   LogOptionCompress,
	}
	mout := io.MultiWriter(os.Stdout, rollingLogger)
	log.SetOutput(mout)

	confFile := flag.String("config", defaultConfigPath, "Path to scdb server configuration file")
	flag.Parse()
	gin.SetMode(gin.ReleaseMode)

	log.Infof("Starting to read config file: %s", *confFile)
	cfg, err := server.NewConfig(*confFile)
	if err != nil {
		log.Fatalf("Failed to create config from %s: %v", *confFile, err)
	}

	// set log level if defined
	if cfg.LogLevel != "" {
		if lvl, err := log.ParseLevel(cfg.LogLevel); err == nil {
			log.SetLevel(lvl)
		}
	}

	log.Info("Starting to connect to database and do bootstrap if necessary...")
	storage.InitPasswordValidation(cfg.PasswordCheck)
	store, err := server.NewDbConnWithBootstrap(&cfg.Storage)
	if err != nil {
		log.Fatalf("Failed to connect to database and bootstrap it: %v", err)
	}

	grmClient := newGrmClient(cfg.GRM)

	engineClient := executor.NewEngineClient(cfg.Engine.ClientTimeout * time.Second)
	svr, err := server.NewServer(cfg, grmClient, store, engineClient)
	if err != nil {
		log.Fatalf("Failed to create scdb server: %v", err)
	}

	if cfg.Protocol == "https" {
		if cfg.TlsConfig.CertFile == "" || cfg.TlsConfig.KeyFile == "" {
			log.Fatalf("Could't start https service without cert file")
		}
		log.Info("Starting to serve request with https...")
		if err := svr.ListenAndServeTLS(cfg.TlsConfig.CertFile, cfg.TlsConfig.KeyFile); err != nil {
			log.Fatalf("Something bad happens to server: %v", err)
		}
	} else {
		log.Info("Starting to serve request with http...")
		if err := svr.ListenAndServe(); err != nil {
			log.Fatalf("Something bad happens to server: %v", err)
		}
	}
}

func newGrmClient(cfg server.GRMConf) grm.Grm {
	grmMode, exist := server.GrmModeType[cfg.GrmMode]
	if !exist {
		log.Fatalf("unknown grm type: %v", cfg.GrmMode)
	}
	switch grmMode {
	case server.ToyGrmMode:
		grmClient, err := toygrm.New(cfg.ToyGrmConf)
		if err != nil {
			log.Fatalf("failed to create grm: %v", err)
		}
		return grmClient
	case server.StdGrmMode:
		httpClient := &http.Client{Timeout: cfg.Timeout}
		return stdgrm.New(httpClient, cfg.Host)
	default:
		log.Fatalf("unknown grm mode %v ", grmMode)
	}
	return nil
}
